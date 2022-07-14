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
    //WARN: work in progress, not usable
    //TODO: write a basic skeleton animator.
    //TODO: offer different strategies:
    // Interpolation Strat
    //   Entirely interpolated: always interpolate everything. most cpu expensive, best results.
    //   Framerate locked: precalculate animations, user provides a "framerate", then interpolate 
    //    up front, write to a map, use the map. Anims will have a frame rate and will take up 
    //    more space but animation interp will not have to be calculated.
    //    Could also LRU cache the animation mixes, would have to parameterize the cache size,
    //    this would mean frequently used animation mixes also wouldnt be interpolated
    //    should also be able to manage many models, so multiple instances can take advantage of the lru and map,
    //    so these things wont grow linearly with instance count
    // Vertex Transform strat
    //   cpu: done in a loop in c++: no special shaders needed therefore no implementation dependencies.
    //    could TRY to make use of worker threads, but would have to introduce params around work size etc.
    //    to get best results on whatever platform.
    //   gpu: final skeleton is uploaded to the gpu, transformations are performed in the vertex shader
    //    offloads work to the gpu but requires special vertex shader.
    //   gpu2: could try to offload interpolation step to gpu? Probably dont have enough instructions to do this
    //    in opengles2.
    //   gpu3: when/ifever I decide to work on vulkan, all this interp work is a great candidate for a compute shader
    //    user provides list of anims to blend and positions in ther timelines:
    //    upload the keyframe pairs for all anims to blend -> interpolation_compute -> vertex_shader -> frag
    class animated_model
    {
        //! hierarchical state of transformations to be applied to vertex data
        struct skeleton
        {
            struct bone
            {
                std::string name;

                graphics_mat4x4_type transform;

                std::unordered_set<bone *> children;
            };

            //! set of unique bones
            std::unordered_set<std::unique_ptr<bone>> all_bones = {};

            //! bones with no parents
            std::unordered_set<bone *> root_bones = {};

            //! check if another skeleton's format matches this one
            bool is_format_same(const skeleton &other) const
            {
                if (all_bones.size() != other.all_bones.size()) 
                    return false;

                auto iThisBone = all_bones.begin();
                auto iThatBone = other.all_bones.begin();
               
                while (iThisBone != all_bones.end())
                {
                    if ((*iThisBone)->name != (*iThatBone)->name)
                        return false;

                    iThisBone++;
                    iThatBone++;
                }

                // TODO: recurse root bones to make sure the trees 
                // have the same structure

                return true;
            }
        };

        //! used to calculate a skeleton, to be used transform 
        /// the vertex data
        class animation
        {
        public:
            using key_frame_collection_type = 
                std::set<std::pair<float/*amount of time*/, skeleton>>;
            
            key_frame_collection_type m_KeyFrames;

            skeleton calculate_skeleton_at(float timeSec)
            {
                auto iLowFrame(m_KeyFrames.begin()), 
                    iHighFrame(m_KeyFrames.begin());

                float interpolationWeight(0);

                for (auto i = m_KeyFrames.begin(); i != m_KeyFrames.end(); ++i)
                {
                    if (timeSec < (*i).first)
                    {
                        iHighFrame = i;
                        
                        auto lowTime  = iLowFrame->first;
                        auto highTime = iHighFrame->first;
                        
                        interpolationWeight =
                            (timeSec - lowTime) / (highTime - lowTime);

                        break;
                    }

                    iLowFrame = i;
                }
                
                skeleton interpolated_skeleton;

                //TODO: interpolate the keyframes
                //decompose translation, rotation, scale
                //interpolate each component write 
                //to the interpolated_skeleton

                return interpolated_skeleton;
            }

            animation(key_frame_collection_type &&aKeyFrames)
            : m_KeyFrames(std::move(aKeyFrames))
            {
                const auto &firstSkeleton = m_KeyFrames.begin()->second;

                for (auto iFrame = m_KeyFrames.begin(); 
                    iFrame != m_KeyFrames.end(); ++iFrame)
                    if (!firstSkeleton.is_format_same(iFrame->second))
                        throw std::invalid_argument("animation: "
                            "skeleton format must be the same in all keyframes");
            }
        };

        //! animations for the model
        std::unordered_map<std::string, animation> m_animations;

        //! vertex data buffer in system memory
        vertex_data m_Buffer = {};
       
        //! vertex data buffer in vram
        std::shared_ptr<gdk::model> m_pModel;

        void animate(const std::unordered_set<std::pair<std::string, float>> 
            &aContributingFrames)
        {
            //TODO: get skelies from all the arged animations, 
            //interpolate them to a final skele
            //iterate vertex data, apply the final skele's 
            //matricies to the data
        }
        void animate(const std::pair<std::string, float> &aFrame)
        {
            //TODO: get skeleton
            //iterate vertex data, apply the final skele's 
            //matricies to the data
        }
    };
}

#endif

