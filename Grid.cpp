#include <SFML/Graphics.hpp>
#include <vector>
#define window_width 1024
#define window_height 1024
#define PI 3.14159265
#include <iostream>

class Object : public sf::Drawable
{
public:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(point, 1, sf::Points);
    }
    float x, y;
    sf::Vertex* point;
    int speed;
    sf::Vector2f direction;
    Object()
    {
        x = rand() % window_width;
        y = rand() % window_height;
        point = new sf::Vertex(sf::Vector2f(x, y), sf::Color::Green);
        speed = rand() % 9 + 1;
        speed *= 1000;
        float next_x = cos(rand() % 360 * PI / 180);
        float next_y = sin(rand() % 360 * PI / 180);
        float z = sqrt(next_x * next_x + next_y * next_y);
        direction = sf::Vector2f(next_x / z, next_y / z);
    }
    void select()
    {
        point->color = sf::Color::Blue;
    }
    void deselect()
    {
        point->color = sf::Color::Green;
    }
    void move(float dt)
    {
        float next_x = direction.x * speed * dt;
        float next_y = direction.y * speed * dt;
        if (x + next_x < 0)
        {
            next_x = -x;
            direction.x = -direction.x;
        }
        else if (x + next_x > window_width)
        {
            next_x = window_width - x;
            direction.x = -direction.x;
        }
        if (y + next_y < 0)
        {
            next_y = -y;
            direction.y = -direction.y;
        }
        else if (y + next_y > window_height)
        {
            next_y = window_height - y;
            direction.y = -direction.y;
        }
        x += next_x;
        y += next_y;
        point->position.x += next_x;
        point->position.y += next_y;
    }
};

class Section
{
public:
    std::vector<Object*> objects;
    Section()
    {}
    void insert(Object* object)
    {
        objects.push_back(object);
    }
    void remove(Object* object)
    {
        for (int i = 0; i < objects.size(); i++)
        {
            if (objects[i] == object)
            {
                objects.erase(objects.begin() + i);
                return;
            }
        }
    }
    void testCollisions()
    {
        if (objects.size() >= 2)
        {
            for (int i = 0; i < objects.size() - 1; i++)
            {
                Object* obj1 = objects[i];
                for (int j = i + 1; j < objects.size(); j++)
                {
                    Object* obj2 = objects[j];
                    if ((obj1->x - obj2->x) * (obj1->x - obj2->x) + (obj1->y - obj2->y) * (obj1->y - obj2->y) < 0.1)
                    {
                        obj1->direction = -obj1->direction;
                        obj2->direction = -obj2->direction;
                        float placeholder = obj1->speed;
                        obj1->speed = obj2->speed;
                        obj2->speed = placeholder;
                    }
                }
            }
        }
    }
};

class Grid : public sf::Drawable
{
public:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(grid);
    }
    int grid_width;
    int grid_height;
    int num_rows;
    int num_cols;
    sf::VertexArray grid;
    std::vector<Section*> sections;
    Grid(int w, int h, std::vector<Object*> objects)
    {
        grid_width = w;
        grid_height = h;
        num_rows = window_height / w;
        num_cols = window_width / h;
        sections.resize((num_rows + 1) * (num_cols + 1));
        for (int i = 0; i < sections.size(); i++)
            sections[i] = new Section();
        for (Object* object : objects)
        {
            int k = object->x / w + (num_cols + 1) * (object->y / h);
            sections[k]->objects.push_back(object);
        }
        grid.resize(2 * num_rows * num_cols);
        grid.setPrimitiveType(sf::Lines);
        for (int i = 0; i < num_rows; i++)
        {
            grid[2 * i].position = sf::Vector2f(0, h * (i + 1));
            grid[2 * i + 1].position = sf::Vector2f(window_width, h * (i + 1));
        }
        for (int i = 0; i < num_cols; i++)
        {
            grid[2 * i + 2 * num_rows].position = sf::Vector2f(w * (i + 1), 0);
            grid[2 * i + 1 + 2 * num_rows].position = sf::Vector2f(w * (i + 1), window_height);
        }
    }
    void insert(Object* object)
    {
        int k = floor(object->x / grid_width) + (num_cols + 1) * floor(object->y / grid_height);
        sections[k]->insert(object);
    }
    void remove(Object* object)
    {
        int k = floor(object->x / grid_width) + (num_cols + 1) * floor(object->y / grid_height);
        sections[k]->remove(object);
    }
    void testCollisions()
    {
        for (Section* section : sections)
            section->testCollisions();
    }
    int getNumCollisions()
    {
        int collisions = 0;
        for (Section* section : sections)
            collisions += section->objects.size() * (section->objects.size() - 1);
        return collisions;
    }
};

void showStats(int num_objects, int collisions, int width, int height, float fps)
{
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Objects: " << num_objects << std::endl;
    std::cout << "Brute Force Collision Checks: " << num_objects * (num_objects - 1) << std::endl;
    std::cout << "Grid Collision Checks: " << collisions << std::endl;
    std::cout << "Grid Dimensions: " << width << "x" << height << std::endl;
    std::cout << "----------------------------" << std::endl;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML works!");
    std::vector<Object*> objects;
    int num_objects = 100000;
    for (int i = 0; i < num_objects; i++)
    {
        Object* object = new Object();
        objects.push_back(object);
    }
    bool toggle = false;
    sf::Clock update;
    int grid_width = 60;
    int grid_height = 60;
    Grid* grid = new Grid(grid_width, grid_height, objects);

    sf::Clock clock;
    float dt = 0;
    float fps;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::F)
                    showStats(objects.size(), grid->getNumCollisions(), grid_width, grid_height, fps);
                if (event.key.code == sf::Keyboard::T)
                    toggle = !toggle;
                if (event.key.code == sf::Keyboard::Num1)
                {
                    grid_width++;
                    grid_height++;
                }
                if (event.key.code == sf::Keyboard::Num2)
                {
                    grid_width--;
                    grid_height--;
                }
            }
        }
        if (update.getElapsedTime().asSeconds() > 0.1)
        {
            for (Object* object : objects)
            {
                grid->remove(object);
                object->move(dt);
                grid->insert(object);
            }
            fps = 1 / clock.restart().asSeconds();
            update.restart();
        }

        window.clear();
        for (Object* object : objects)
            window.draw(*object);
        if (toggle)
            window.draw(*grid);
        window.display();

        dt = clock.restart().asSeconds();
    }

    return 0;
}