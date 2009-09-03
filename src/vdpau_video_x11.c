/*
 *  vdpau_video.h - VDPAU backend for VA API (X11 rendering)
 *
 *  vdpau-video (C) 2009 Splitted-Desktop Systems
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "sysdeps.h"
#include "vdpau_video.h"
#include "vdpau_video_x11.h"
#include "utils.h"

#define DEBUG 1
#include "debug.h"


// Returns X drawable dimensions
static void
get_drawable_size(
    Display      *display,
    Drawable      drawable,
    unsigned int *pwidth,
    unsigned int *pheight
)
{
    Window rootwin;
    int x, y;
    unsigned int width, height, border_width, depth;
    XGetGeometry(display, drawable, &rootwin,
                 &x, &y, &width, &height, &border_width, &depth);
    if (pwidth)
        *pwidth = width;
    if (pheight)
        *pheight = height;
}

// Create output surface
VASurfaceID
create_output_surface(
    vdpau_driver_data_t *driver_data,
    uint32_t             width,
    uint32_t             height
)
{
    int i;
    int surface = object_heap_allocate(&driver_data->output_heap);
    if (surface < 0)
        return VA_INVALID_SURFACE;

    object_output_p obj_output = VDPAU_OUTPUT(surface);
    ASSERT(obj_output);
    if (obj_output == NULL)
        return VA_INVALID_SURFACE;

    obj_output->drawable                = None;
    obj_output->width                   = 0;
    obj_output->height                  = 0;
    obj_output->vdp_flip_queue          = VDP_INVALID_HANDLE;
    obj_output->vdp_flip_target         = VDP_INVALID_HANDLE;
    obj_output->output_surface_width    = width;
    obj_output->output_surface_height   = height;
    obj_output->current_output_surface  = 0;
    for (i = 0; i < VDPAU_MAX_OUTPUT_SURFACES; i++)
        obj_output->vdp_output_surfaces[i] = VDP_INVALID_HANDLE;

    VADriverContextP const ctx = driver_data->va_context;
    uint32_t display_width, display_height;
    display_width  = DisplayWidth(ctx->x11_dpy, ctx->x11_screen);
    display_height = DisplayHeight(ctx->x11_dpy, ctx->x11_screen);
    if (obj_output->output_surface_width < display_width)
        obj_output->output_surface_width = display_width;
    if (obj_output->output_surface_height < display_height)
        obj_output->output_surface_height = display_height;

    for (i = 0; i < VDPAU_MAX_OUTPUT_SURFACES; i++) {
        VdpStatus vdp_status;
        VdpOutputSurface vdp_output_surface;
        vdp_status = vdpau_output_surface_create(driver_data,
                                                 driver_data->vdp_device,
                                                 VDP_RGBA_FORMAT_B8G8R8A8,
                                                 obj_output->output_surface_width,
                                                 obj_output->output_surface_height,
                                                 &vdp_output_surface);

        if (vdp_status != VDP_STATUS_OK) {
            destroy_output_surface(driver_data, surface);
            return VA_INVALID_SURFACE;
        }

        obj_output->vdp_output_surfaces[i] = vdp_output_surface;
    }

    return surface;
}

// Destroy output surface
void
destroy_output_surface(vdpau_driver_data_t *driver_data, VASurfaceID surface)
{
    if (surface == VA_INVALID_SURFACE)
        return;

    object_output_p obj_output = VDPAU_OUTPUT(surface);
    ASSERT(obj_output);
    if (obj_output == NULL)
        return;

    destroy_flip_queue(driver_data, obj_output);

    int i;
    for (i = 0; i < VDPAU_MAX_OUTPUT_SURFACES; i++) {
        VdpOutputSurface vdp_output_surface = obj_output->vdp_output_surfaces[i];
        if (vdp_output_surface != VDP_INVALID_HANDLE) {
            vdpau_output_surface_destroy(driver_data, vdp_output_surface);
            obj_output->vdp_output_surfaces[i] = VDP_INVALID_HANDLE;
        }
    }
    object_heap_free(&driver_data->output_heap, (object_base_p)obj_output);
}

// Create presentation queue
VAStatus
create_flip_queue(vdpau_driver_data_t *driver_data, object_output_p obj_output)
{
    VdpPresentationQueue vdp_flip_queue = VDP_INVALID_HANDLE;
    VdpPresentationQueueTarget vdp_flip_target = VDP_INVALID_HANDLE;
    VdpStatus vdp_status;

    vdp_status =
        vdpau_presentation_queue_target_create_x11(driver_data,
                                                   driver_data->vdp_device,
                                                   obj_output->drawable,
                                                   &vdp_flip_target);

    if (vdp_status != VDP_STATUS_OK)
        return vdpau_get_VAStatus(driver_data, vdp_status);

    vdp_status =
        vdpau_presentation_queue_create(driver_data,
                                        driver_data->vdp_device,
                                        vdp_flip_target,
                                        &vdp_flip_queue);

    if (vdp_status != VDP_STATUS_OK) {
        vdpau_presentation_queue_target_destroy(driver_data, vdp_flip_target);
        return vdpau_get_VAStatus(driver_data, vdp_status);
    }

    obj_output->vdp_flip_queue  = vdp_flip_queue;
    obj_output->vdp_flip_target = vdp_flip_target;
    return VA_STATUS_SUCCESS;
}

// Destroy presentation queue
void
destroy_flip_queue(vdpau_driver_data_t *driver_data, object_output_p obj_output)
{
    if (obj_output->vdp_flip_queue != VDP_INVALID_HANDLE) {
        vdpau_presentation_queue_destroy(driver_data,
                                         obj_output->vdp_flip_queue);
        obj_output->vdp_flip_queue = VDP_INVALID_HANDLE;
    }

    if (obj_output->vdp_flip_target != VDP_INVALID_HANDLE) {
        vdpau_presentation_queue_target_destroy(driver_data,
                                                obj_output->vdp_flip_target);
        obj_output->vdp_flip_target = VDP_INVALID_HANDLE;
    }
}

// Render surface to a Drawable
VAStatus
put_surface(
    vdpau_driver_data_t *driver_data,
    VASurfaceID          surface,
    Drawable             drawable,
    unsigned int         drawable_width,
    unsigned int         drawable_height,
    const VdpRect       *source_rect,
    const VdpRect       *target_rect
)
{
    object_surface_p obj_surface = VDPAU_SURFACE(surface);
    ASSERT(obj_surface);
    if (obj_surface == NULL)
        return VA_STATUS_ERROR_INVALID_SURFACE;

    object_context_p obj_context = VDPAU_CONTEXT(obj_surface->va_context);
    ASSERT(obj_context);
    if (obj_context == NULL)
        return VA_STATUS_ERROR_INVALID_CONTEXT;

    object_output_p obj_output = VDPAU_OUTPUT(obj_context->output_surface);
    ASSERT(obj_output);
    if (obj_output == NULL)
        return VA_STATUS_ERROR_INVALID_SURFACE;

    obj_surface->va_surface_status  = VASurfaceReady;
    obj_surface->vdp_output_surface = VDP_INVALID_HANDLE;

    VdpStatus vdp_status;
    VAStatus va_status;

    if (obj_output->drawable != drawable) {
        destroy_flip_queue(driver_data, obj_output);

        obj_output->drawable = drawable;
        va_status = create_flip_queue(driver_data, obj_output);
        if (va_status != VA_STATUS_SUCCESS)
            return va_status;
    }

    if (obj_output->width  != drawable_width ||
        obj_output->height != drawable_height) {
        obj_output->width   = drawable_width;
        obj_output->height  = drawable_height;

        /* XXX: re-create output surfaces incrementally here? */
    }

    ASSERT(obj_output->drawable == drawable);
    ASSERT(obj_output->vdp_flip_queue != VDP_INVALID_HANDLE);
    ASSERT(obj_output->vdp_flip_target != VDP_INVALID_HANDLE);

    VdpOutputSurface vdp_output_surface;
    vdp_output_surface = obj_output->vdp_output_surfaces[obj_output->current_output_surface];

    VdpTime dummy_time;
    vdp_status = vdpau_presentation_queue_block_until_surface_idle(driver_data,
                                                                   obj_output->vdp_flip_queue,
                                                                   vdp_output_surface,
                                                                   &dummy_time);

    if (vdp_status != VDP_STATUS_OK)
        return vdpau_get_VAStatus(driver_data, vdp_status);

    vdp_status = vdpau_video_mixer_render(driver_data,
                                          obj_context->vdp_video_mixer,
                                          VDP_INVALID_HANDLE,
                                          NULL,
                                          VDP_VIDEO_MIXER_PICTURE_STRUCTURE_FRAME,
                                          0, NULL,
                                          obj_surface->vdp_surface,
                                          0, NULL,
                                          source_rect,
                                          vdp_output_surface,
                                          NULL,
                                          target_rect,
                                          0, NULL);

    if (vdp_status != VDP_STATUS_OK)
        return vdpau_get_VAStatus(driver_data, vdp_status);

    uint32_t clip_width, clip_height;
    clip_width  = MIN(obj_output->output_surface_width, drawable_width);
    clip_height = MIN(obj_output->output_surface_height, drawable_height);
    vdp_status  = vdpau_presentation_queue_display(driver_data,
                                                   obj_output->vdp_flip_queue,
                                                   vdp_output_surface,
                                                   clip_width,
                                                   clip_height,
                                                   0);

    if (vdp_status != VDP_STATUS_OK)
        return vdpau_get_VAStatus(driver_data, vdp_status);

    obj_surface->va_surface_status     = VASurfaceDisplaying;
    obj_surface->vdp_output_surface    = vdp_output_surface;
    obj_output->current_output_surface = (obj_output->current_output_surface + 1) % VDPAU_MAX_OUTPUT_SURFACES;
    return VA_STATUS_SUCCESS;
}

// vaPutSurface
VAStatus
vdpau_PutSurface(
    VADriverContextP    ctx,
    VASurfaceID         surface,
    Drawable            draw,
    short               srcx,
    short               srcy,
    unsigned short      srcw,
    unsigned short      srch,
    short               destx,
    short               desty,
    unsigned short      destw,
    unsigned short      desth,
    VARectangle        *cliprects,
    unsigned int        number_cliprects,
    unsigned int        flags
)
{
    VDPAU_DRIVER_DATA_INIT;

    /* XXX: no clip rects supported */
    if (cliprects || number_cliprects > 0)
        return VA_STATUS_ERROR_INVALID_PARAMETER;

    /* XXX: only support VA_FRAME_PICTURE */
    if (flags)
        return VA_STATUS_ERROR_FLAG_NOT_SUPPORTED;

    unsigned int w, h;
    get_drawable_size(ctx->x11_dpy, draw, &w, &h);

    VdpRect src_rect, dst_rect;
    src_rect.x0 = srcx;
    src_rect.y0 = srcy;
    src_rect.x1 = src_rect.x0 + srcw;
    src_rect.y1 = src_rect.y0 + srch;
    dst_rect.x0 = destx;
    dst_rect.y0 = desty;
    dst_rect.x1 = dst_rect.x0 + destw;
    dst_rect.y1 = dst_rect.y0 + desth;
    return put_surface(driver_data, surface, draw, w, h, &src_rect, &dst_rect);
}