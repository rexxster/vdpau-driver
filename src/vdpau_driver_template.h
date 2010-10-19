/*
 *  vdpau_driver_template.h - VDPAU driver initialization template
 *
 *  vdpau-video (C) 2009-2010 Splitted-Desktop Systems
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

#undef  CONCAT_
#define CONCAT_(x, y)   x##y
#undef  CONCAT
#define CONCAT(x, y)    CONCAT_(x, y)
#undef  FUNC
#define FUNC(name)      CONCAT(CONCAT(CONCAT(vdpau_,name),_),VA_INIT_SUFFIX)

#undef  VA_INIT_CHECK_VERSION
#define VA_INIT_CHECK_VERSION(major, minor, micro)      \
    (VA_INIT_VERSION_MAJOR > (major) ||                 \
     (VA_INIT_VERSION_MAJOR == (major) &&               \
      VA_INIT_VERSION_MINOR > (minor)) ||               \
     (VA_INIT_VERSION_MAJOR == (major) &&               \
      VA_INIT_VERSION_MINOR == (minor) &&               \
      VA_INIT_VERSION_MICRO >= (micro)))

#undef  VA_INIT_CHECK_VERSION_SDS
#define VA_INIT_CHECK_VERSION_SDS(major, minor, micro, sds)             \
    (VA_INIT_CHECK_VERSION(major, minor, (micro)+1) ||                  \
     (VA_CHECK_VERSION(major, minor, micro) && VA_SDS_VERSION >= (sds)))

#ifndef VA_INIT_SUFFIX
#define VA_INIT_SUFFIX  Current
#define VA_INIT_CURRENT 1
#else
#define VA_INIT_CURRENT 0
#endif

#ifndef VA_INIT_VERSION_SDS
#define VA_INIT_VERSION_SDS 0
#endif

#ifndef VA_INIT_GLX
#define VA_INIT_GLX     0
#endif

#if VA_INIT_CURRENT
#define VA_DRIVER_VTABLE        VADriverVTable
#define VA_DRIVER_CONTEXT       VADriverContext
#define VA_DRIVER_CONTEXT_P     VADriverContextP
#else
#define VA_DRIVER_VTABLE        CONCAT(VADriverVTable_,VA_INIT_SUFFIX)
#define VA_DRIVER_VTABLE_GLX_P  CONCAT(VADriverVTableGLX_,VA_INIT_SUFFIX)
#define VA_DRIVER_CONTEXT       CONCAT(VADriverContext_,VA_INIT_SUFFIX)
#define VA_DRIVER_CONTEXT_P     CONCAT(VADriverContextP_,VA_INIT_SUFFIX)

typedef struct VA_DRIVER_CONTEXT *VA_DRIVER_CONTEXT_P;

/* Driver VTable */
struct VA_DRIVER_VTABLE {
	VAStatus (*vaTerminate) ( VA_DRIVER_CONTEXT_P ctx );

	VAStatus (*vaQueryConfigProfiles) (
		VA_DRIVER_CONTEXT_P ctx,
		VAProfile *profile_list,	/* out */
		int *num_profiles			/* out */
	);

	VAStatus (*vaQueryConfigEntrypoints) (
		VA_DRIVER_CONTEXT_P ctx,
		VAProfile profile,
		VAEntrypoint  *entrypoint_list,	/* out */
		int *num_entrypoints			/* out */
	);

	VAStatus (*vaGetConfigAttributes) (
		VA_DRIVER_CONTEXT_P ctx,
		VAProfile profile,
		VAEntrypoint entrypoint,
		VAConfigAttrib *attrib_list,	/* in/out */
		int num_attribs
	);

	VAStatus (*vaCreateConfig) (
		VA_DRIVER_CONTEXT_P ctx,
		VAProfile profile, 
		VAEntrypoint entrypoint, 
		VAConfigAttrib *attrib_list,
		int num_attribs,
		VAConfigID *config_id		/* out */
	);

	VAStatus (*vaDestroyConfig) (
		VA_DRIVER_CONTEXT_P ctx,
		VAConfigID config_id
	);

	VAStatus (*vaQueryConfigAttributes) (
		VA_DRIVER_CONTEXT_P ctx,
		VAConfigID config_id, 
		VAProfile *profile,		/* out */
		VAEntrypoint *entrypoint, 	/* out */
		VAConfigAttrib *attrib_list,	/* out */
		int *num_attribs		/* out */
	);

	VAStatus (*vaCreateSurfaces) (
		VA_DRIVER_CONTEXT_P ctx,
		int width,
		int height,
		int format,
		int num_surfaces,
		VASurfaceID *surfaces		/* out */
	);

	VAStatus (*vaDestroySurfaces) (
		VA_DRIVER_CONTEXT_P ctx,
		VASurfaceID *surface_list,
		int num_surfaces
	);

	VAStatus (*vaCreateContext) (
		VA_DRIVER_CONTEXT_P ctx,
		VAConfigID config_id,
		int picture_width,
		int picture_height,
		int flag,
		VASurfaceID *render_targets,
		int num_render_targets,
		VAContextID *context		/* out */
	);

	VAStatus (*vaDestroyContext) (
		VA_DRIVER_CONTEXT_P ctx,
		VAContextID context
	);

	VAStatus (*vaCreateBuffer) (
		VA_DRIVER_CONTEXT_P ctx,
		VAContextID context,		/* in */
		VABufferType type,		/* in */
		unsigned int size,		/* in */
		unsigned int num_elements,	/* in */
		void *data,			/* in */
		VABufferID *buf_id		/* out */
	);

	VAStatus (*vaBufferSetNumElements) (
		VA_DRIVER_CONTEXT_P ctx,
		VABufferID buf_id,	/* in */
		unsigned int num_elements	/* in */
	);

	VAStatus (*vaMapBuffer) (
		VA_DRIVER_CONTEXT_P ctx,
		VABufferID buf_id,	/* in */
		void **pbuf         /* out */
	);

	VAStatus (*vaUnmapBuffer) (
		VA_DRIVER_CONTEXT_P ctx,
		VABufferID buf_id	/* in */
	);

	VAStatus (*vaDestroyBuffer) (
		VA_DRIVER_CONTEXT_P ctx,
		VABufferID buffer_id
	);

	VAStatus (*vaBeginPicture) (
		VA_DRIVER_CONTEXT_P ctx,
		VAContextID context,
		VASurfaceID render_target
	);

	VAStatus (*vaRenderPicture) (
		VA_DRIVER_CONTEXT_P ctx,
		VAContextID context,
		VABufferID *buffers,
		int num_buffers
	);

	VAStatus (*vaEndPicture) (
		VA_DRIVER_CONTEXT_P ctx,
		VAContextID context
	);

	VAStatus (*vaSyncSurface) (
		VA_DRIVER_CONTEXT_P ctx,
		VASurfaceID render_target
	);

	VAStatus (*vaQuerySurfaceStatus) (
		VA_DRIVER_CONTEXT_P ctx,
		VASurfaceID render_target,
		VASurfaceStatus *status	/* out */
	);

	VAStatus (*vaPutSurface) (
    		VA_DRIVER_CONTEXT_P ctx,
		VASurfaceID surface,
		Drawable draw, /* X Drawable */
		short srcx,
		short srcy,
		unsigned short srcw,
		unsigned short srch,
		short destx,
		short desty,
		unsigned short destw,
		unsigned short desth,
		VARectangle *cliprects, /* client supplied clip list */
		unsigned int number_cliprects, /* number of clip rects in the clip list */
		unsigned int flags /* de-interlacing flags */
	);

	VAStatus (*vaQueryImageFormats) (
		VA_DRIVER_CONTEXT_P ctx,
		VAImageFormat *format_list,        /* out */
		int *num_formats           /* out */
	);

	VAStatus (*vaCreateImage) (
		VA_DRIVER_CONTEXT_P ctx,
		VAImageFormat *format,
		int width,
		int height,
		VAImage *image     /* out */
	);

	VAStatus (*vaDeriveImage) (
		VA_DRIVER_CONTEXT_P ctx,
		VASurfaceID surface,
		VAImage *image     /* out */
	);

	VAStatus (*vaDestroyImage) (
		VA_DRIVER_CONTEXT_P ctx,
		VAImageID image
	);
	
	VAStatus (*vaSetImagePalette) (
	        VA_DRIVER_CONTEXT_P ctx,
	        VAImageID image,
	        /*
                 * pointer to an array holding the palette data.  The size of the array is
                 * num_palette_entries * entry_bytes in size.  The order of the components
                 * in the palette is described by the component_order in VAImage struct
                 */
                unsigned char *palette
	);
	
	VAStatus (*vaGetImage) (
		VA_DRIVER_CONTEXT_P ctx,
		VASurfaceID surface,
		int x,     /* coordinates of the upper left source pixel */
		int y,
		unsigned int width, /* width and height of the region */
		unsigned int height,
		VAImageID image
	);

	VAStatus (*vaPutImage) (
		VA_DRIVER_CONTEXT_P ctx,
		VASurfaceID surface,
		VAImageID image,
		int src_x,
		int src_y,
		unsigned int src_width,
		unsigned int src_height,
		int dest_x,
		int dest_y,
		unsigned int dest_width,
		unsigned int dest_height
	);

	VAStatus (*vaQuerySubpictureFormats) (
		VA_DRIVER_CONTEXT_P ctx,
		VAImageFormat *format_list,        /* out */
		unsigned int *flags,       /* out */
		unsigned int *num_formats  /* out */
	);

	VAStatus (*vaCreateSubpicture) (
		VA_DRIVER_CONTEXT_P ctx,
		VAImageID image,
		VASubpictureID *subpicture   /* out */
	);

	VAStatus (*vaDestroySubpicture) (
		VA_DRIVER_CONTEXT_P ctx,
		VASubpictureID subpicture
	);

        VAStatus (*vaSetSubpictureImage) (
                VA_DRIVER_CONTEXT_P ctx,
                VASubpictureID subpicture,
                VAImageID image
        );

	VAStatus (*vaSetSubpictureChromakey) (
		VA_DRIVER_CONTEXT_P ctx,
		VASubpictureID subpicture,
		unsigned int chromakey_min,
		unsigned int chromakey_max,
		unsigned int chromakey_mask
	);

	VAStatus (*vaSetSubpictureGlobalAlpha) (
		VA_DRIVER_CONTEXT_P ctx,
		VASubpictureID subpicture,
		float global_alpha 
	);

	VAStatus (*vaAssociateSubpicture) (
		VA_DRIVER_CONTEXT_P ctx,
		VASubpictureID subpicture,
		VASurfaceID *target_surfaces,
		int num_surfaces,
		short src_x, /* upper left offset in subpicture */
		short src_y,
		unsigned short src_width,
		unsigned short src_height,
		short dest_x, /* upper left offset in surface */
		short dest_y,
		unsigned short dest_width,
		unsigned short dest_height,
		/*
		 * whether to enable chroma-keying or global-alpha
		 * see VA_SUBPICTURE_XXX values
		 */
		unsigned int flags
	);

	VAStatus (*vaDeassociateSubpicture) (
		VA_DRIVER_CONTEXT_P ctx,
		VASubpictureID subpicture,
		VASurfaceID *target_surfaces,
		int num_surfaces
	);

	VAStatus (*vaQueryDisplayAttributes) (
		VA_DRIVER_CONTEXT_P ctx,
		VADisplayAttribute *attr_list,	/* out */
		int *num_attributes		/* out */
        );

	VAStatus (*vaGetDisplayAttributes) (
		VA_DRIVER_CONTEXT_P ctx,
		VADisplayAttribute *attr_list,	/* in/out */
		int num_attributes
        );
        
        VAStatus (*vaSetDisplayAttributes) (
		VA_DRIVER_CONTEXT_P ctx,
                VADisplayAttribute *attr_list,
                int num_attributes
        );

#if VA_INIT_CHECK_VERSION(0,31,1)
        /* used by va trace */        
        VAStatus (*vaBufferInfo) (
                VA_DRIVER_CONTEXT_P ctx,
                VAContextID context, /* in */
                VABufferID buf_id, /* in */
                VABufferType *type,    /* out */
                unsigned int *size,    /* out */
                unsigned int *num_elements /* out */
        );

        /* lock/unlock surface for external access */    
        VAStatus (*vaLockSurface) (
		VA_DRIVER_CONTEXT_P ctx,
                VASurfaceID surface,
                unsigned int *fourcc, /* out  for follow argument */
                unsigned int *luma_stride,
                unsigned int *chroma_u_stride,
                unsigned int *chroma_v_stride,
                unsigned int *luma_offset,
                unsigned int *chroma_u_offset,
                unsigned int *chroma_v_offset,
                unsigned int *buffer_name, /* if it is not NULL, assign the low lever
                                            * surface buffer name
                                            */
                void **buffer /* if it is not NULL, map the surface buffer for
                                * CPU access
                                */
        );
    
        VAStatus (*vaUnlockSurface) (
		VA_DRIVER_CONTEXT_P ctx,
                VASurfaceID surface
        );

        /* Optional: GLX support hooks */
        void *glx;
#else
        /* device specific */
	VAStatus (*vaCreateSurfaceFromCIFrame) (
		VA_DRIVER_CONTEXT_P ctx,
		unsigned long frame_id,
		VASurfaceID *surface		/* out */
	);
    
    
        VAStatus (*vaCreateSurfaceFromV4L2Buf) (
		VA_DRIVER_CONTEXT_P ctx,
                int v4l2_fd,         /* file descriptor of V4L2 device */
                struct v4l2_format *v4l2_fmt,       /* format of V4L2 */
                struct v4l2_buffer *v4l2_buf,       /* V4L2 buffer */
                VASurfaceID *surface	           /* out */
        );
    
        VAStatus (*vaCopySurfaceToBuffer) (
		VA_DRIVER_CONTEXT_P ctx,
                VASurfaceID surface,
                unsigned int *fourcc, /* out  for follow argument */
                unsigned int *luma_stride,
                unsigned int *chroma_u_stride,
                unsigned int *chroma_v_stride,
                unsigned int *luma_offset,
                unsigned int *chroma_u_offset,
                unsigned int *chroma_v_offset,
                void **buffer
        );
#endif
};

/* Driver context */
struct VA_DRIVER_CONTEXT {
    void *pDriverData;
    struct VA_DRIVER_VTABLE vtable;
#if VA_INIT_CHECK_VERSION(0,31,1)
    void *vtable_tpi; /* the structure is malloc-ed */
#endif

    Display *native_dpy;
    int x11_screen;
    int version_major;
    int version_minor;
    int max_profiles;
    int max_entrypoints;
    int max_attributes;
    int max_image_formats;
    int max_subpic_formats;
    int max_display_attributes;
    const char *str_vendor;

    void *handle;			/* dlopen handle */
    
    void *dri_state;
#if VA_INIT_CHECK_VERSION(0,31,1)
    void *glx;                         /* opaque for GLX code */
#endif
};
#endif

// Check for VA/GLX changes from libVA API >= 0.31.0-sds2
#if VA_INIT_GLX
#if VA_INIT_CHECK_VERSION_SDS(0,31,0,2)
typedef struct VADriverVTableGLX *VA_DRIVER_VTABLE_GLX_P;
#else
typedef struct VA_DRIVER_VTABLE  *VA_DRIVER_VTABLE_GLX_P;
#endif

static inline VA_DRIVER_VTABLE_GLX_P FUNC(GetVTableGLX)(VA_DRIVER_CONTEXT_P ctx)
{
#if VA_INIT_CHECK_VERSION_SDS(0,31,0,6)
    /* Upstream VA-API 0.31.1 or SDS >= 0.31.0-sds6 */
    VA_DRIVER_VTABLE_GLX_P vtable_glx = ctx->vtable.glx;

    if (!vtable_glx) {
        vtable_glx = calloc(1, sizeof(*vtable_glx));
        if (!vtable_glx)
            return NULL;
        ctx->vtable.glx = vtable_glx;
    }
    return vtable_glx;
#elif VA_INIT_CHECK_VERSION_SDS(0,31,0,2)
    /* SDS >= 0.31.0-sds2 */
    return &ctx->vtable.glx;
#else
    /* Any other VA-API version 0.31.0 or lower */
    return &ctx->vtable;
#endif
}

static inline void FUNC(ReleaseVTableGLX)(VA_DRIVER_CONTEXT_P ctx)
{
#if VA_INIT_CHECK_VERSION_SDS(0,31,0,6)
    free(ctx->vtable.glx);
    ctx->vtable.glx = NULL;
#endif
}
#endif

static VAStatus FUNC(Terminate)(VA_DRIVER_CONTEXT_P ctx)
{
    VDPAU_DRIVER_DATA_INIT;

    vdpau_common_Terminate(driver_data);

#if VA_INIT_GLX
    FUNC(ReleaseVTableGLX)(ctx);
#endif

    free(ctx->pDriverData);
    ctx->pDriverData = NULL;

    return VA_STATUS_SUCCESS;
}

static VAStatus FUNC(Initialize)(VA_DRIVER_CONTEXT_P ctx)
{
    struct vdpau_driver_data *driver_data;

    driver_data = calloc(1, sizeof(*driver_data));
    if (!driver_data)
        return VA_STATUS_ERROR_ALLOCATION_FAILED;

    ctx->pDriverData            = driver_data;
    driver_data->x11_dpy        = ctx->native_dpy;
    driver_data->x11_screen     = ctx->x11_screen;

    VAStatus va_status = vdpau_common_Initialize(driver_data);
    if (va_status != VA_STATUS_SUCCESS) {
        FUNC(Terminate)(ctx);
        return va_status;
    }

    ctx->version_major          = VA_INIT_VERSION_MAJOR;
    ctx->version_minor          = VA_INIT_VERSION_MINOR;
    ctx->max_profiles           = VDPAU_MAX_PROFILES;
    ctx->max_entrypoints        = VDPAU_MAX_ENTRYPOINTS;
    ctx->max_attributes         = VDPAU_MAX_CONFIG_ATTRIBUTES;
    ctx->max_image_formats      = VDPAU_MAX_IMAGE_FORMATS;
    ctx->max_subpic_formats     = VDPAU_MAX_SUBPICTURE_FORMATS;
    ctx->max_display_attributes = VDPAU_MAX_DISPLAY_ATTRIBUTES;
    ctx->str_vendor             = driver_data->va_vendor;

    memset(&ctx->vtable, 0, sizeof(ctx->vtable));
    ctx->vtable.vaTerminate                     = FUNC(Terminate);
    ctx->vtable.vaQueryConfigEntrypoints        = vdpau_QueryConfigEntrypoints;
    ctx->vtable.vaQueryConfigProfiles           = vdpau_QueryConfigProfiles;
    ctx->vtable.vaQueryConfigEntrypoints        = vdpau_QueryConfigEntrypoints;
    ctx->vtable.vaQueryConfigAttributes         = vdpau_QueryConfigAttributes;
    ctx->vtable.vaCreateConfig                  = vdpau_CreateConfig;
    ctx->vtable.vaDestroyConfig                 = vdpau_DestroyConfig;
    ctx->vtable.vaGetConfigAttributes           = vdpau_GetConfigAttributes;
    ctx->vtable.vaCreateSurfaces                = vdpau_CreateSurfaces;
    ctx->vtable.vaDestroySurfaces               = vdpau_DestroySurfaces;
    ctx->vtable.vaCreateContext                 = vdpau_CreateContext;
    ctx->vtable.vaDestroyContext                = vdpau_DestroyContext;
    ctx->vtable.vaCreateBuffer                  = vdpau_CreateBuffer;
    ctx->vtable.vaBufferSetNumElements          = vdpau_BufferSetNumElements;
    ctx->vtable.vaMapBuffer                     = vdpau_MapBuffer;
    ctx->vtable.vaUnmapBuffer                   = vdpau_UnmapBuffer;
    ctx->vtable.vaDestroyBuffer                 = vdpau_DestroyBuffer;
    ctx->vtable.vaBeginPicture                  = vdpau_BeginPicture;
    ctx->vtable.vaRenderPicture                 = vdpau_RenderPicture;
    ctx->vtable.vaEndPicture                    = vdpau_EndPicture;
#if VA_INIT_CHECK_VERSION(0,31,0)
    ctx->vtable.vaSyncSurface                   = vdpau_SyncSurface2;
#else
    ctx->vtable.vaSyncSurface                   = vdpau_SyncSurface3;
#endif
    ctx->vtable.vaQuerySurfaceStatus            = vdpau_QuerySurfaceStatus;
    ctx->vtable.vaPutSurface                    = vdpau_PutSurface;
    ctx->vtable.vaQueryImageFormats             = vdpau_QueryImageFormats;
    ctx->vtable.vaCreateImage                   = vdpau_CreateImage;
    ctx->vtable.vaDeriveImage                   = vdpau_DeriveImage;
    ctx->vtable.vaDestroyImage                  = vdpau_DestroyImage;
    ctx->vtable.vaSetImagePalette               = vdpau_SetImagePalette;
    ctx->vtable.vaGetImage                      = vdpau_GetImage;
#if VA_INIT_CHECK_VERSION(0,31,0)
    ctx->vtable.vaPutImage                      = vdpau_PutImage_full;
#else
    ctx->vtable.vaPutImage                      = vdpau_PutImage;
    ctx->vtable.vaPutImage2                     = vdpau_PutImage_full;
#endif
    ctx->vtable.vaQuerySubpictureFormats        = vdpau_QuerySubpictureFormats;
    ctx->vtable.vaCreateSubpicture              = vdpau_CreateSubpicture;
    ctx->vtable.vaDestroySubpicture             = vdpau_DestroySubpicture;
    ctx->vtable.vaSetSubpictureImage            = vdpau_SetSubpictureImage;
    ctx->vtable.vaSetSubpictureChromakey        = vdpau_SetSubpictureChromakey;
    ctx->vtable.vaSetSubpictureGlobalAlpha      = vdpau_SetSubpictureGlobalAlpha;
#if VA_INIT_CHECK_VERSION(0,31,0)
    ctx->vtable.vaAssociateSubpicture           = vdpau_AssociateSubpicture_full;
#else
    ctx->vtable.vaAssociateSubpicture           = vdpau_AssociateSubpicture;
    ctx->vtable.vaAssociateSubpicture2          = vdpau_AssociateSubpicture_full;
#endif
    ctx->vtable.vaDeassociateSubpicture         = vdpau_DeassociateSubpicture;
    ctx->vtable.vaQueryDisplayAttributes        = vdpau_QueryDisplayAttributes;
    ctx->vtable.vaGetDisplayAttributes          = vdpau_GetDisplayAttributes;
    ctx->vtable.vaSetDisplayAttributes          = vdpau_SetDisplayAttributes;
#if VA_INIT_CHECK_VERSION(0,31,1)
    ctx->vtable.vaBufferInfo                    = vdpau_BufferInfo;
    ctx->vtable.vaLockSurface                   = vdpau_LockSurface;
    ctx->vtable.vaUnlockSurface                 = vdpau_UnlockSurface;
#else
#if VA_INIT_CHECK_VERSION(0,30,0)
    ctx->vtable.vaCreateSurfaceFromCIFrame      = vdpau_CreateSurfaceFromCIFrame;
    ctx->vtable.vaCreateSurfaceFromV4L2Buf      = vdpau_CreateSurfaceFromV4L2Buf;
    ctx->vtable.vaCopySurfaceToBuffer           = vdpau_CopySurfaceToBuffer;
#else
    ctx->vtable.vaSetSubpicturePalette          = vdpau_SetSubpicturePalette;
    ctx->vtable.vaDbgCopySurfaceToBuffer        = vdpau_DbgCopySurfaceToBuffer;
#endif
#endif

#if VA_INIT_GLX
    VA_DRIVER_VTABLE_GLX_P const glx_vtable     = FUNC(GetVTableGLX)(ctx);
    if (!glx_vtable)
        return VA_STATUS_ERROR_ALLOCATION_FAILED;
    glx_vtable->vaCreateSurfaceGLX              = vdpau_CreateSurfaceGLX;
    glx_vtable->vaDestroySurfaceGLX             = vdpau_DestroySurfaceGLX;
    glx_vtable->vaCopySurfaceGLX                = vdpau_CopySurfaceGLX;
#endif
    return VA_STATUS_SUCCESS;
}

#undef VA_INIT_CURRENT
#undef VA_INIT_VERSION_MAJOR
#undef VA_INIT_VERSION_MINOR
#undef VA_INIT_VERSION_MICRO
#undef VA_INIT_VERSION_SDS
#undef VA_INIT_SUFFIX
#undef VA_INIT_GLX

#undef VA_DRIVER_VTABLE
#undef VA_DRIVER_VTABLE_GLX_P
#undef VA_DRIVER_CONTEXT
#undef VA_DRIVER_CONTEXT_P