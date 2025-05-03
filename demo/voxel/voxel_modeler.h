// © Joseph Cameron - All Rights Reserved

#ifndef GDK_GFX_EXT_VOXEL_MODELER_H
#define GDK_GFX_EXT_VOXEL_MODELER_H

#include <gdk/graphics_context.h>

namespace gdk::graphics::ext 
{
    /// \brief generates a volumetic-pixel model of a 3D array
    ///
    /// TODO:
    ///  implement greedy meshing in update_vertex_data
    ///
    template<size_t data_size = 16>
    class voxel_modeler final
    {
    public:
        void set_voxel_data(size_t aX, size_t aY, size_t aZ, size_t aValue)
        {
            m_VoxelData[aX][aY][aZ] = aValue;
        }
        
        size_t value_at(size_t aX, size_t aY, size_t aZ) const
        {
            return m_VoxelData[aX][aY][aZ];
        }

        // Per voxel rendering. works but should replace with greedy meshing implementation
        void update_vertex_data()
        {
            m_VertexBuffer.clear();

            gdk::vertex_data buffer;
                            
            for (size_t x(0); x < data_size; ++x)
            {
                for (size_t y(0); y < data_size; ++y) 
                {
                    for (size_t z(0); z < data_size; ++z)
                    {
                        if (m_VoxelData[x][y][z] > 0) 
                        {
                            auto pNorthNeighbour = m_pNorthNeighbour.lock(); 
                            size_t northNeighbourValue = z == data_size - 1 
                                ? pNorthNeighbour
                                    ? pNorthNeighbour->value_at(x,y,0)
                                    : 0
                                : m_VoxelData[x][y][z + 1];

                            auto pSouthNeighbour = m_pSouthNeighbour.lock(); 
                            size_t southNeighbourValue = z == 0 
                                ? pSouthNeighbour
                                    ? pSouthNeighbour->value_at(x,y,data_size - 1)
                                    : 0 
                                : m_VoxelData[x][y][z - 1];

                            auto pEastNeighbour = m_pEastNeighbour.lock(); 
                            size_t eastNeighbourValue = x == data_size - 1 
                                ? pEastNeighbour
                                    ? pEastNeighbour->value_at(0,y,z)
                                    : 0 
                                : m_VoxelData[x + 1][y][z];
                            
                            auto pWestNeighbour = m_pWestNeighbour.lock(); 
                            size_t westNeighbourValue = x == 0 
                                ? pWestNeighbour
                                    ? pWestNeighbour->value_at(data_size - 1, y, z)
                                    : 0
                                : m_VoxelData[x - 1][y][z];
                            
                            auto pTopNeighbour = m_pTopNeighbour.lock(); 
                            size_t topNeighbourValue = y == data_size - 1 
                                ? pTopNeighbour
                                    ? pTopNeighbour->value_at(x, 0, z)
                                    : 0
                                : m_VoxelData[x][y + 1][z];
                            
                            auto pBottomNeighbour = m_pBottomNeighbour.lock(); 
                            size_t bottomNeighbourValue = y == 0 
                                ? pBottomNeighbour
                                    ? pBottomNeighbour->value_at(x, data_size - 1, z)
                                    : 0
                                : m_VoxelData[x][y - 1][z];

                            auto fX(static_cast<graphics_floating_point_type>(x));
                            auto fY(static_cast<graphics_floating_point_type>(y));
                            auto fZ(static_cast<graphics_floating_point_type>(z));

                            if (!northNeighbourValue)
                            {
                                gdk::vertex_data data(northData);

                                data.transform_position({fX, fY, fZ});
                                data.transform_position({0}, {}, {fX, fY, fZ}, "a_VoxelPosition");

                                buffer += data;
                                m_VertexBuffer.push_back(data);
                            }

                            if (!southNeighbourValue)
                            {
                                gdk::vertex_data data(southData);

                                data.transform_position({fX, fY, fZ});
                                data.transform_position({0}, {}, {fX, fY, fZ}, "a_VoxelPosition");

                                buffer += data;
                                m_VertexBuffer.push_back(data);
                            }

                            if (!eastNeighbourValue)
                            {
                                gdk::vertex_data data(eastData);

                                data.transform_position({fX, fY, fZ});
                                data.transform_position({0}, {}, {fX, fY, fZ}, "a_VoxelPosition");

                                buffer += data;
                                m_VertexBuffer.push_back(data);
                            }
                            
                            if (!westNeighbourValue)
                            {
                                gdk::vertex_data data(westData);

                                data.transform_position({fX, fY, fZ});
                                data.transform_position({0}, {}, {fX, fY, fZ}, "a_VoxelPosition");

                                buffer += data;
                                m_VertexBuffer.push_back(data);
                            }
                           
                            if (!topNeighbourValue)
                            {
                                gdk::vertex_data data(topData);

                                data.transform_position({fX, fY, fZ});
                                data.transform_position({0}, {}, {fX, fY, fZ}, "a_VoxelPosition");

                                buffer += data;
                                m_VertexBuffer.push_back(data);
                            }

                            if (!bottomNeighbourValue)
                            {
                                gdk::vertex_data data(bottomData);

                                data.transform_position({fX, fY, fZ});
                                data.transform_position({0}, {}, {fX, fY, fZ}, "a_VoxelPosition");

                                buffer += data;
                                m_VertexBuffer.push_back(data);
                            }
                        }
                    }
                }
            }
        }

        const gdk::vertex_data &vertex_data() const
        {
            return m_VertexBuffer;
        }

        void set_north_neighbour(std::weak_ptr<voxel_modeler> aNeighbour)
        {
            m_pNorthNeighbour = aNeighbour;
        }

        void set_south_neighbour(std::weak_ptr<voxel_modeler> aNeighbour)
        {
            m_pSouthNeighbour = aNeighbour;
        }

        void set_east_neighbour(std::weak_ptr<voxel_modeler> aNeighbour)
        {
            m_pEastNeighbour = aNeighbour;
        }
        
        void set_west_neighbour(std::weak_ptr<voxel_modeler> aNeighbour)
        {
            m_pWestNeighbour = aNeighbour;
        }

        void set_top_neighbour(std::weak_ptr<voxel_modeler> aNeighbour)
        {
            m_pTopNeighbour = aNeighbour;
        ;}
        
        void set_bottom_neighbour(std::weak_ptr<voxel_modeler> aNeighbour)
        {
            m_pBottomNeighbour = aNeighbour;
        }

        //! constructs a voxel modeler
        voxel_modeler(std::shared_ptr<gdk::graphics::context> pContext)
        {}

    private:
        gdk::vertex_data m_VertexBuffer;

        std::weak_ptr<voxel_modeler> m_pNorthNeighbour;
        std::weak_ptr<voxel_modeler> m_pSouthNeighbour;
        std::weak_ptr<voxel_modeler> m_pEastNeighbour;
        std::weak_ptr<voxel_modeler> m_pWestNeighbour;
        std::weak_ptr<voxel_modeler> m_pTopNeighbour;
        std::weak_ptr<voxel_modeler> m_pBottomNeighbour;

        std::array<std::array<std::array<size_t, data_size>, data_size>, data_size> m_VoxelData = {{{0}}};

        std::vector<float> uvData {
            1, 0,
            0, 0,
            0, 1,
            1, 0,
            0, 1,
            1, 1,
        };

        const gdk::vertex_data northData {
            {
                { 
                    "a_Position",
                    {
                        {
                            1.0f, 1.0f, 1.0f,
                            0.0f, 1.0f, 1.0f,
                            0.0f, 0.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            0.0f, 0.0f, 1.0f,
                            1.0f, 0.0f, 1.0f,
                        },
                        3
                    }
                },
                { 
                    "a_UV",
                    {
                        uvData,
                        2
                    }
                },
                {
                    "a_VoxelPosition",
                    {
                        {
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                        },
                        3
                    }
                }
            }
        };

        const gdk::vertex_data southData {
            {
                { 
                    "a_Position",
                    {
                        {
                            1.0f, 1.0f, 0.0f,
                            0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f,
                            1.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, 0.0f,
                        },
                        3
                    }
                },
                { 
                    "a_UV",
                    {
                        uvData,
                        2
                    }
                },
                {
                    "a_VoxelPosition",
                    {
                        {
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                        },
                        3
                    }
                }
            }
        };
        
        const gdk::vertex_data eastData {
            {
                { 
                    "a_Position",
                    {
                        {
                            1.0f, 0.0f, 0.0f,
                            1.0f, 1.0f, 0.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 0.0f, 0.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 0.0f, 1.0f,
                        },
                        3
                    }
                },
                { 
                    "a_UV",
                    {
                        uvData,
                        2
                    }
                },
                {
                    "a_VoxelPosition",
                    {
                        {
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                        },
                        3
                    }
                }
            }
        };
        
        const gdk::vertex_data westData {
            {
                { 
                    "a_Position",
                    {
                        {
                            0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f,
                            0.0f, 1.0f, 1.0f,
                            0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 1.0f,
                            0.0f, 0.0f, 1.0f,
                        },
                        3
                    }
                },
                { 
                    "a_UV",
                    {
                        uvData,
                        2
                    }
                },
                {
                    "a_VoxelPosition",
                    {
                        {
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                        },
                        3
                    }
                }
            }
        };
        
        const gdk::vertex_data bottomData {
            {
                { 
                    "a_Position",
                    {
                        {
                            0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, 1.0f,
                            0.0f, 0.0f, 0.0f,
                            1.0f, 0.0f, 1.0f,
                            0.0f, 0.0f, 1.0f,
                        },
                        3
                    }
                },
                { 
                    "a_UV",
                    {
                        uvData,
                        2
                    }
                },
                {
                    "a_VoxelPosition",
                    {
                        {
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                        },
                        3
                    }
                }
            }
        };

        const gdk::vertex_data topData {
            {
                { 
                    "a_Position",
                    {
                        {
                            0.0f, 1.0f, 0.0f,
                            1.0f, 1.0f, 0.0f,
                            1.0f, 1.0f, 1.0f,
                            0.0f, 1.0f, 0.0f,
                            1.0f, 1.0f, 1.0f,
                            0.0f, 1.0f, 1.0f,
                        },
                        3
                    }
                },
                { 
                    "a_UV",
                    {
                        uvData,
                        2
                    }
                },
                {
                    "a_VoxelPosition",
                    {
                        {
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                        },
                        3
                    }
                }
            }
        };
    };
}

#endif

