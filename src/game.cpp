#include <game.hpp>

#include <engine/window.hpp>
#include <engine/time.hpp>
#include <engine/debug.hpp>

#include <framework/world.hpp>

#include <engine/serial.hpp>
#include <engine/memory.hpp>
#include <engine/file.hpp>

#include <cereal/archives/json.hpp>
#include <fstream>

using namespace rttr;

enum class color
{
    red = 1,
    green = 2,
    blue = 3
};

struct point2d
{
    point2d() {}
    point2d(int x_, int y_) : x(x_), y(y_) {}
    int x = 0;
    int y = 0;
};

struct shape
{
    shape() {}
    shape(std::string n) : name(n) {}

    void set_visible(bool v) { visible = v; }
    bool get_visible() const { return visible; }

    color color_ = color::blue;
    std::string name = "";
    point2d position;
    //std::map<color, point2d> dictionary;

    RTTR_ENABLE()
private:
    bool visible = false;
};

struct circle : shape
{
    circle() {}
    circle(std::string n) : shape(n) {}

    double radius = 5.2;
    std::vector<point2d> points;

    int no_serialize = 100;

    RTTR_ENABLE(shape)
};

struct rect : shape
{
    rect() {}
    rect(std::string n) : shape(n) {}

    double width = 5.2;
    double height = 5.2;

    RTTR_ENABLE(shape)
};

RTTR_REGISTRATION
{
    rttr::registration::class_<shape>("shape")
        .constructor()(rttr::policy::ctor::as_object)
        .property("visible", &shape::get_visible, &shape::set_visible)
        .property("color", &shape::color_)
        .property("name", &shape::name)
        .property("position", &shape::position)
        //.property("dictionary", &shape::dictionary)
    ;

    rttr::registration::class_<circle>("circle")
        .constructor()(rttr::policy::ctor::as_std_shared_ptr)
        .property("radius", &circle::radius)
        .property("points", &circle::points)
        .property("no_serialize", &circle::no_serialize)
        (
            metadata("NO_SERIALIZE", true)
        )
        ;

    rttr::registration::class_<rect>("rect")
        .constructor()(rttr::policy::ctor::as_std_shared_ptr)
        .property("width", &rect::width)
        .property("height", &rect::height)
        ;

    rttr::registration::class_<point2d>("point2d")
        .constructor()(rttr::policy::ctor::as_object)
        .property("x", &point2d::x)
        .property("y", &point2d::y)
        ;


    rttr::registration::enumeration<color>("color")
        (
            value("red", color::red),
            value("blue", color::blue),
            value("green", color::green)
        );

    //https://github.com/rttrorg/rttr/issues/81
    //https://github.com/rttrorg/rttr/pull/132
    rttr::type::register_wrapper_converter_for_base_classes<SharedPtr<circle>>();
    rttr::type::register_wrapper_converter_for_base_classes<SharedPtr<rect>>();
}

SkyPiGame::SkyPiGame()
{
}

bool SkyPiGame::CanAddScene()
{
    return true;
}

void SkyPiGame::Configure()
{
    {
        std::ofstream os(File::Get().GetPath("[assets]/data.json"));
        cereal::JSONOutputArchive archive(os);

        SharedPtr<circle> s1{ new circle("Circle #1") };
        s1->set_visible(true);
        s1->points = std::vector<point2d>(2, point2d(1, 1));
        s1->points[1].x = 23;
        s1->points[1].y = 42;
        s1->position.x = 12;
        s1->position.y = 66;
        s1->radius = 15.123;
        s1->color_ = color::red;
        // additional braces are needed for a VS 2013 bug
        //s1.dictionary = { { {color::green, {1, 2} }, {color::blue, {3, 4} }, {color::red, {5, 6} } } };
        s1->no_serialize = 12345;

        SharedPtr<rect> s2{ new rect("Rect #2") };
        s2->width = 10;
        s2->height = 20;

        List<SharedPtr<shape>> obj{};
        obj.emplace_back(s1);
        obj.emplace_back(s2);
        archive(cereal::make_nvp("shapes", obj));

        //SharedPtr<shape> obj{ new circle(c_1) };
        //archive(cereal::make_nvp("shapes", obj));

        {
            std::stringstream os{};
            cereal::JSONOutputArchive oa(os);
            oa(cereal::make_nvp("shapes", obj));
            BX_LOGI(Log, "save:\n{}", os.str());
        }
    }

    {
        std::ifstream is(File::Get().GetPath("[assets]/data.json"));
        cereal::JSONInputArchive archive(is);

        List<SharedPtr<shape>> obj{};
        archive(cereal::make_nvp("shapes", obj));

        //SharedPtr<shape> obj{};
        //archive(cereal::make_nvp("shape", obj));

        {
            std::stringstream os{};
            cereal::JSONOutputArchive oa(os);
            oa(cereal::make_nvp("shapes", obj));
            BX_LOGI(Log, "load:\n{}", os.str());
        }
    }
}

bool SkyPiGame::Initialize()
{
    //m_gameScene = CreateScene<World>();
    //SetActiveScene(m_gameScene);

    return true;
}

void SkyPiGame::Update()
{
}

void SkyPiGame::Render()
{
}

void SkyPiGame::Shutdown()
{
    //Graphics::Get().DestroyBuffer(m_constantBuffer);
    //m_terrain.Shutdown();
}