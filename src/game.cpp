#include <game.hpp>
#include <engine/window.hpp>
#include <engine/time.hpp>
#include <engine/debug.hpp>
#include <framework/world.hpp>

#include <engine/serial.hpp>

using namespace rttr;

enum class color
{
    red,
    green,
    blue
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
    //std::vector<point2d> points;

    int no_serialize = 100;

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
        .constructor()(rttr::policy::ctor::as_object)
        .property("radius", &circle::radius)
        //.property("points", &circle::points)
        .property("no_serialize", &circle::no_serialize)
        (
            metadata("NO_SERIALIZE", true)
        )
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
        circle c_1("Circle #1");
        shape& my_shape = c_1;

        c_1.set_visible(true);
        //c_1.points = std::vector<point2d>(2, point2d(1, 1));
        //c_1.points[1].x = 23;
        //c_1.points[1].y = 42;

        c_1.position.x = 12;
        c_1.position.y = 66;

        c_1.radius = 5.123;
        c_1.color_ = color::red;

        // additional braces are needed for a VS 2013 bug
        //c_1.dictionary = { { {color::green, {1, 2} }, {color::blue, {3, 4} }, {color::red, {5, 6} } } };

        c_1.no_serialize = 12345;

        std::vector<shape*> shapes{};
        shapes.push_back(new circle(c_1));
        shapes.push_back(new circle(c_1));

        Serial::Save(shapes, "[assets]/test.json"); // serialize the circle to 'json_string'
    }

    //std::cout << "Circle: c_1:\n" << json_string << std::endl;
    
    //circle c_2("Circle #2"); // create a new empty circle
    //shape& my_shape = c_2;
    
    std::vector<shape*> shapes{};
    Serial::Load(shapes, "[assets]/test.json"); // deserialize it with the content of 'c_1'
    //std::cout << "\n############################################\n" << std::endl;
    //
    //std::cout << "Circle c_2:\n" << io::to_json(c_2) << std::endl;
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