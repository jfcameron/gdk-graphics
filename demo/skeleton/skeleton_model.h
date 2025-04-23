// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_ANIMATED_MODEL_H
#define GDK_GFX_ANIMATED_MODEL_H

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <gdk/graphics_context.h>
#include <gdk/vertex_data.h>
#include <gdk/graphics_types.h>

namespace gdk
{
    struct bone_data
    {
        std::string name;
        graphics_mat4x4_type transform;
        std::vector<std::string> children;
        
        bone_data(
            std::string aName,
            graphics_mat4x4_type aTransform,
            std::vector<std::string> aChildren)
        :name(aName)
        ,transform(aTransform)
        ,children(aChildren)
        {}
    };
    
    struct bone
    {
        graphics_mat4x4_type localTransform;
        graphics_mat4x4_type transform;

        std::unordered_set<bone *> children;

        bone *parent;
    };

    //TODO bones shouldnt have "names", they should have an index, so they can be encoded into vertex data
    struct skeleton
    {
        std::unordered_map<std::string, bone> boneMap;

        bone *rootBone;

        //This works but proably should be removed? Its expensive and shouldnt be used excessively
        void set_local_transform(std::string aBoneName, graphics_mat4x4_type aTransform);

        skeleton(const std::vector<bone_data> &aBoneData);
    };

    struct animation
    {
        using key_frame_collection_type = std::map<float, skeleton>;

        key_frame_collection_type m_KeyFrames;

        //Time should probably be normalized
        skeleton get_frame(const float aTime) const;

        animation(key_frame_collection_type aKeyFrames);
    };
}

#endif

