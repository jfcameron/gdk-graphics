// © 2019 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include "test_include.h"

//#include <gdk/floatuniformcollection.h>
#include <gdk/shaderprogram.h>

using namespace gdk;

TEST_CASE("floatuniformcollection tests", "[floatuniformcollection]")
{
    initGL();

    auto pShader = std::shared_ptr<ShaderProgram>(ShaderProgram::AlphaCutOff);
  
    //std::cout << "floatuniformcollection shdare: " << pShader->getHandle() << std::endl;

    REQUIRE(!jfc::glGetError());

    /*auto pShader2 = std::shared_ptr<ShaderProgram>(ShaderProgram::AlphaCutOff);
    REQUIRE(!jfc::glGetError());*/

    //FloatUniformCollection a;
    
    SECTION("insert")
    {
        //const auto brightness = std::make_shared<FloatUniformCollection::uniform_type::element_type>(0.5f);

        //a.insert("brightness", brightness);
    }

    SECTION("bind")
    {
        //a.bind(pShader->getHandle());
    }
    
    REQUIRE(!jfc::glGetError());
}

