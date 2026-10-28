// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/graphics/exception.h>
#include <gdk/graphics/model_data.h>

using namespace gdk::graphics;

namespace {
    [[nodiscard]] model_data quad() {
        return model_data({{"a_Position", {{0, 0, 0,  1, 0, 0,  1, 1, 0,  0, 1, 0}, 3}}});
    }

    [[nodiscard]] std::vector<model_data::index_value_type> quad_indexes() {
        return {0, 1, 2, 0, 2, 3};
    }

    [[nodiscard]] std::size_t vertices_of(const model_data &aModel) {
        return aModel.get_attribute_data("a_Position").components().size() / 3;
    }
}

TEST_CASE("model_data indexing", "[gdk::graphics::model_data]")
{
    SECTION("a model is unindexed until it is given indices")
    {
        const auto model = quad();

        REQUIRE_FALSE(model.indexed());
        REQUIRE(model.indexes().empty());
    }

    SECTION("indices are kept in the order they are given")
    {
        auto model = quad();

        model.set_indexes(quad_indexes());

        REQUIRE(model.indexed());
        REQUIRE(model.indexes() == quad_indexes());
    }

    SECTION("the constructor taking indices agrees with the setter")
    {
        const model_data built({{"a_Position", {{0, 0, 0,  1, 0, 0,  1, 1, 0,  0, 1, 0}, 3}}},
            {0, 1, 2, 0, 2, 3});

        REQUIRE(built.indexes() == quad_indexes());
    }

    SECTION("**an index past the end of the vertices is refused**")
    {
        auto model = quad();

        REQUIRE_THROWS(model.set_indexes({0, 1, 4}));
        REQUIRE_THROWS(model.set_indexes({0, 1, 2, 0, 2, 9}));
        REQUIRE_NOTHROW(model.set_indexes({0, 1, 3}));
    }

    SECTION("an empty list returns the model to storage order")
    {
        auto model = quad();

        model.set_indexes(quad_indexes());
        model.set_indexes({});

        REQUIRE_FALSE(model.indexed());
    }

    SECTION("clear removes the indices along with everything else")
    {
        auto model = quad();

        model.set_indexes(quad_indexes());
        model.clear();

        REQUIRE_FALSE(model.indexed());
    }
}

TEST_CASE("model_data::push_back with indices", "[gdk::graphics::model_data]")
{
    SECTION("**appended indices are offset to name the appended vertices**")
    {
        auto target = quad();
        target.set_indexes(quad_indexes());

        auto source = quad();
        source.set_indexes(quad_indexes());

        target.push_back(source);

        REQUIRE(vertices_of(target) == 8);
        REQUIRE(target.indexes().size() == 12);

        const std::vector<model_data::index_value_type> expected{
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7};

        REQUIRE(target.indexes() == expected);
    }

    SECTION("appending unindexed to indexed gives the newcomer the indices it implies")
    {
        auto target = quad();
        target.set_indexes(quad_indexes());

        target.push_back(quad());

        REQUIRE(target.indexed());
        REQUIRE(target.indexes().size() == 6 + 4);
        REQUIRE(target.indexes().back() == 7);
    }

    SECTION("appending indexed to unindexed indexes what was already there")
    {
        auto target = quad();

        auto source = quad();
        source.set_indexes(quad_indexes());

        target.push_back(source);

        REQUIRE(target.indexed());

        REQUIRE(target.indexes().size() == 4 + 6);
        REQUIRE(target.indexes().front() == 0);
    }

    SECTION("appending unindexed to unindexed leaves both unindexed")
    {
        auto target = quad();

        target.push_back(quad());

        REQUIRE_FALSE(target.indexed());
        REQUIRE(vertices_of(target) == 8);
    }

    SECTION("appending onto an empty model carries the indices across")
    {
        model_data target;

        auto source = quad();
        source.set_indexes(quad_indexes());

        target.push_back(source);

        REQUIRE(target.indexes() == quad_indexes());
    }
}

TEST_CASE("model_data sorting an indexed model", "[gdk::graphics::model_data]")
{
    SECTION("**sorting permutes indices and leaves the vertices where they are**")
    {
        model_data model({{"a_Position", {{
            0, 0, -10,  1, 0, -10,  0, 1, -10,          // far
            0, 0,  -1,  1, 0,  -1,  0, 1,  -1}, 3}}});  // near

        model.set_indexes({0, 1, 2, 3, 4, 5});

        const auto before = model.get_attribute_data("a_Position").components();

        model.sort_by_nearest_triangle({0, 0, 0}, matrix4x4_type::identity);

        REQUIRE(model.get_attribute_data("a_Position").components() == before);

        const std::vector<model_data::index_value_type> nearestFirst{3, 4, 5, 0, 1, 2};

        REQUIRE(model.indexes() == nearestFirst);
    }
}

TEST_CASE("sorting an index buffer that is not whole triangles", "[gdk::graphics::model_data]")
{
    model_data model = model_data::make_quad();

    model.set_indexes({0, 1, 2, 3, 4, 5, 0});

    model.sort_by_nearest_triangle({0, 0, 0}, matrix4x4_type::identity);

    REQUIRE(model.indexes().size() == 7);
}

TEST_CASE("sorting an unindexed model that is not whole triangles",
    "[gdk::graphics::model_data]")
{
    model_data model = model_data::make_quad();

    const auto before = model.vertex_count();

    REQUIRE(before % 3 == 0);

    model_data::attribute_collection_type single;

    const model_data source = model_data::make_quad();

    for (const auto &[name, data] : source.attributes()) {
        const auto components = data.number_of_components_per_attribute();

        single.emplace(name, attribute_data(
            std::span<const component_type>(data.components().data(), components), components));
    }

    model.push_back(model_data(std::move(single)));

    REQUIRE(model.vertex_count() == before + 1);

    model.sort_by_nearest_triangle({0, 0, 0}, matrix4x4_type::identity);

    REQUIRE(model.vertex_count() == before + 1);
}

TEST_CASE("appending attribute data that has no width of its own",
    "[gdk::graphics::attribute_data]")
{
    attribute_data fresh;

    REQUIRE(fresh.number_of_components_per_attribute() == 0);
    REQUIRE(fresh.number_of_attributes_in_component_data() == 0);

    fresh.push_back(attribute_data({0, 0, 1, 0, 0, 1}, 2));

    REQUIRE(fresh.number_of_components_per_attribute() == 2);
    REQUIRE(fresh.components().size() == 6);
    REQUIRE(fresh.number_of_attributes_in_component_data() == 3);

    SECTION("and appending a second time keeps the width it settled on")
    {
        fresh.push_back(attribute_data({1, 1}, 2));

        REQUIRE(fresh.number_of_components_per_attribute() == 2);
        REQUIRE(fresh.number_of_attributes_in_component_data() == 4);
    }

    SECTION("**two attributes of different width cannot become one**")
    {
        REQUIRE_THROWS(fresh.push_back(attribute_data({0, 0, 0}, 3)));
    }
}

TEST_CASE("appending models that describe different attributes",
    "[gdk::graphics::model_data]")
{
    model_data plain({{"a_Position", attribute_data({0, 0, 0, 1, 0, 0, 0, 1, 0}, 3)}});

    model_data textured({
        {"a_Position", attribute_data({0, 0, 1, 1, 0, 1, 0, 1, 1}, 3)},
        {"a_UV", attribute_data({0, 0, 1, 0, 0, 1}, 2)}});

    REQUIRE_THROWS(plain.push_back(textured));
    REQUIRE_THROWS(textured.push_back(plain));

    SECTION("neither side is modified by an append that was refused")
    {
        REQUIRE(plain.vertex_count() == 3);
        REQUIRE(plain.attributes().size() == 1);

        REQUIRE(textured.vertex_count() == 3);
        REQUIRE(textured.attributes().size() == 2);
    }

    SECTION("and matching attributes still append")
    {
        model_data a = model_data::make_quad();

        const auto before = a.vertex_count();

        a.push_back(model_data::make_quad());

        REQUIRE(a.vertex_count() == before * 2);

        for (const auto &[name, data] : a.attributes())
            REQUIRE(data.number_of_attributes_in_component_data() == a.vertex_count());
    }

    SECTION("appending onto an empty model still adopts it whole")
    {
        model_data empty;

        empty.push_back(textured);

        REQUIRE(empty.attributes().size() == 2);
        REQUIRE(empty.vertex_count() == 3);
    }
}

TEST_CASE("model_data overwrite writes into the model it is called on", "[model_data]") {
    auto subject = model_data::make_quad();

    model_data patch({{"a_UV", attribute_data({7.f, 7.f}, 2)}});

    subject.overwrite("a_UV", 1, patch);

    REQUIRE(subject.get_attribute_data("a_UV").components()[2] == 7.f);
    REQUIRE(subject.get_attribute_data("a_UV").components()[3] == 7.f);
}

TEST_CASE("model_data refuses attributes that are not whole vertices, and takes none at all", "[model_data]") {
    REQUIRE(model_data(model_data::attribute_collection_type{}).vertex_count() == 0);

    REQUIRE_THROWS_AS(model_data({{"a_Position", attribute_data({0.f, 0.f, 0.f, 0.f}, 3)}}), exception);
    REQUIRE_THROWS_AS(model_data({{"a_Position", attribute_data({0.f, 0.f, 0.f}, 0)}}), exception);
}

TEST_CASE("model_data checks component counts before reading by them", "[model_data]") {
    SECTION("sorting by triangle needs 3 component positions") {
        model_data flat({{"a_Position", attribute_data({0.f, 0.f, 1.f, 0.f, 0.f, 1.f}, 2)}}, {0, 1, 2});

        REQUIRE_THROWS_AS(flat.sort_by_nearest_triangle({0, 0, 0}, matrix4x4_type::identity), exception);
    }

    SECTION("a transform of positions made ragged afterwards is refused") {
        auto quad = model_data::make_quad();

        quad.get_attribute_data("a_Position").components().push_back(1.f);

        REQUIRE_THROWS_AS(quad.transform("a_Position", vector3_type{1, 0, 0}), exception);
    }
}

TEST_CASE("model_data push_back changes nothing when it refuses", "[model_data]") {
    std::vector<float> positions(3 * 65535, 0.f);
    std::vector<model_data::index_value_type> indexes{0, 1, 2};

    model_data large({{"a_Position", attribute_data(positions, 3)}}, std::move(indexes));

    const model_data more({{"a_Position", attribute_data({0.f, 0.f, 0.f, 0.f, 0.f, 0.f}, 3)}});

    REQUIRE_THROWS_AS(large.push_back(more), exception);

    REQUIRE(large.vertex_count() == 65535);
    REQUIRE(large.get_attribute_data("a_Position").components().size() == 3 * 65535);
    REQUIRE(large.indexes().size() == 3);

    SECTION("an attribute of another width is refused, also before anything changes") {
        model_data quad = model_data::make_quad();
        const model_data wide({{"a_Position", attribute_data({0.f, 0.f, 0.f, 0.f}, 4)},
            {"a_UV", attribute_data({0.f, 0.f}, 2)}});

        REQUIRE_THROWS_AS(quad.push_back(wide), exception);
        REQUIRE(quad.vertex_count() == 6);
    }
}
