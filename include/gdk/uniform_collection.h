// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_UNIFORMCOLLECTION_H
#define GDK_GFX_UNIFORMCOLLECTION_H

#include <gdk/opengl.h>

#include <iosfwd>
#include <map>
#include <string>

namespace gdk
{
    /// \brief is used to supply data to shaderprograms.
    ///
    /// \detailed It is a baseclass that manages, binds and unbinds a collection of T to a
    /// shaderprogram
    ///
    /// \todo Cache uniform locations. currently uniform locations are resolved at every bind
    /// \todo replace string with string. no need for a deep copy
    /// TODO replace pure virtuals with bind/clear functors then final the template.
    template<typename uniform_t> class uniform_collection
    {
    public:
        using uniform_type = uniform_t;

    protected:
        std::map<std::string, uniform_t> m_Map = {}; //!< internal map
            
    public:
        //! adds a uniform_t to the map at aName, overwriting if a uniform_t is already there.
        void insert(const std::string &aName, const uniform_t &aItem)
        {
            if (m_Map.find(aName) != m_Map.end()) m_Map.erase(aName);
            
            m_Map.insert({aName, aItem});
        }

        //! returns a uniform_t at aName
        /// \Warning I dont think the failure case is safe for all types uniform_t. Need to unit test different categories of uniform_t
        /*uniform_t get(const std::string &aName) const
        {
            return m_Map.find(aName);
        }*/

        /*erase(iter)*/

        void clear()
        {
            m_Map.clear();
        }

        //! Uploads uniform data to currently used program
        virtual void bind(const GLuint aProgramHandle) const = 0;

        //! zeroes uniform data in currently used program
        virtual void unbind(const GLuint aProgramHandle) const = 0;
            
        uniform_collection &operator=(const uniform_collection &) = delete;
        uniform_collection &operator=(uniform_collection &&) = delete;
      
        uniform_collection() = default;
        uniform_collection(const uniform_collection &) = default;
        uniform_collection(uniform_collection &&) = default;
        virtual ~uniform_collection() = default;
    };
}

#endif
