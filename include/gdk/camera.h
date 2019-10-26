// © 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_CAMERA_H
#define GDK_GFX_CAMERA_H

namespace gdk
{
    /// \brief basic camera.
    class camera
    {
    public:
        /// \brief Describes camera clear behaviour
        enum class ClearMode 
        {
            Nothing,  //!< Do not clear any buffers
            Color,    //!< Clear the color buffer
            DepthOnly //!< Clear the Depth buffer
        };                          
        
    };
}

#endif
