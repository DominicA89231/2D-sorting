#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>

#define PI 3.1415926535
#define window_width 1080
#define window_height 1080

class Area : public sf::Drawable
{
public:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(bounds);
    }
    void update(sf::Vector2i mouse_pos)
    {
        bounds[0].position = sf::Vector2f(mouse_pos.x - width / 2, mouse_pos.y - length / 2);
        bounds[1].position = sf::Vector2f(mouse_pos.x + width / 2, mouse_pos.y - length / 2);
        bounds[2].position = sf::Vector2f(mouse_pos.x + width / 2, mouse_pos.y + length / 2);
        bounds[3].position = sf::Vector2f(mouse_pos.x - width / 2, mouse_pos.y + length / 2);
        bounds[6].position = sf::Vector2f(mouse_pos.x - width / 2, mouse_pos.y - length / 2);
        bounds[4].position = sf::Vector2f(mouse_pos.x + width / 2, mouse_pos.y - length / 2);
        bounds[5].position = sf::Vector2f(mouse_pos.x + width / 2, mouse_pos.y + length / 2);
        bounds[7].position = sf::Vector2f(mouse_pos.x - width / 2, mouse_pos.y + length / 2);
        x1 = mouse_pos.x - width / 2;
        y1 = mouse_pos.y - length / 2;
        x2 = mouse_pos.x + width / 2;
        y2 = mouse_pos.y + length / 2;
    }
    int width, length;
    int x1, y1, x2, y2;
    sf::VertexArray bounds;
    Area(int w, int l)
    {
        width = w;
        length = l;
        bounds.resize(8);
        bounds.setPrimitiveType(sf::Lines);
    }
};

class Point : public sf::Drawable
{
public:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(point, 1, sf::Points);
    }
    float x, y;
    sf::Vertex* point;
    sf::Vector2f direction;
    float speed;
    Point(int _x, int _y) 
    {
        x = _x;
        y = _y;
        point = new sf::Vertex(sf::Vector2f(x, y), sf::Color::Green);
        speed = rand() % 10 + 1;
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
    void move(bool random)
    {
        if (random)
        {
            float next_x = cos(rand() % 360 * PI / 180) * speed;
            float next_y = sin(rand() % 360 * PI / 180) * speed;
            if (x + next_x < 0)
                next_x = x;
            else if (x + next_x > window_width)
                next_x = window_width - x;
            if (y + next_y < 0)
                next_y = y;
            else if (y + next_y > window_height)
                next_y = window_height - y;
            x += next_x;
            y += next_y;
            point->position.x += next_x;
            point->position.y += next_y;
        }
        else
        {
            float next_x = direction.x * speed;
            float next_y = direction.y * speed;
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
    }
};

class Quad : public sf::Drawable 
{
public:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(bounds);
        if (top_left_tree != NULL)
            top_left_tree->draw(target, states);
        if (top_right_tree != NULL)
            top_right_tree->draw(target, states);
        if (bottom_left_tree != NULL)
            bottom_left_tree->draw(target, states);
        if (bottom_right_tree != NULL)
            bottom_right_tree->draw(target, states);
    }
    Quad(int _capacity, int _depth)
    {
        capacity = _capacity;
        depth = _depth;
        top_left_tree = NULL;
        top_right_tree = NULL;
        bottom_left_tree = NULL;
        bottom_right_tree = NULL;
    }
    Quad(std::vector<Point*> _points, int _capacity)
    {
        points = _points;
        top_left = sf::Vector2i(0, 0);
        bottom_right = sf::Vector2i(window_width, window_height);
        top_left_tree = NULL;
        top_right_tree = NULL;
        bottom_left_tree = NULL;
        bottom_right_tree = NULL;
        capacity = _capacity;
        if (points.size() > capacity)
            subdivide();
        setBounds();
    }
    std::vector<Point*> points;

    int capacity;
    int depth;
    sf::Vector2i top_left    ; // top left of boundary of quadtree
    sf::Vector2i bottom_right; // bottom right of boundary of quadtree

    sf::VertexArray bounds;

    Quad* top_left_tree;
    Quad* top_right_tree;
    Quad* bottom_left_tree;
    Quad* bottom_right_tree;

    void setBounds() 
    {
        bounds.resize(8);
        bounds.setPrimitiveType(sf::Lines);
        bounds[0].position = sf::Vector2f(top_left.x, top_left.y);
        bounds[1].position = sf::Vector2f(bottom_right.x, top_left.y);
        bounds[2].position = sf::Vector2f(bottom_right.x, bottom_right.y);
        bounds[3].position = sf::Vector2f(top_left.x, bottom_right.y);
        bounds[6].position = sf::Vector2f(top_left.x, top_left.y);
        bounds[4].position = sf::Vector2f(bottom_right.x, top_left.y);
        bounds[5].position = sf::Vector2f(bottom_right.x, bottom_right.y);
        bounds[7].position = sf::Vector2f(top_left.x, bottom_right.y);
    }
    void subdivide() 
    {
        if (depth < 10)
        {
            top_left_tree = new Quad(capacity, depth + 1);
            top_left_tree->top_left = top_left;
            top_left_tree->bottom_right = sf::Vector2i((bottom_right.x + top_left.x) / 2, (bottom_right.y + top_left.y) / 2);
            top_left_tree->setBounds();

            top_right_tree = new Quad(capacity, depth + 1);
            top_right_tree->top_left = sf::Vector2i((bottom_right.x + top_left.x) / 2, top_left.y);
            top_right_tree->bottom_right = sf::Vector2i(bottom_right.x, (bottom_right.y + top_left.y) / 2);
            top_right_tree->setBounds();

            bottom_left_tree = new Quad(capacity, depth + 1);
            bottom_left_tree->top_left = sf::Vector2i(top_left.x, (bottom_right.y + top_left.y) / 2);
            bottom_left_tree->bottom_right = sf::Vector2i((bottom_right.x + top_left.x) / 2, bottom_right.y);
            bottom_left_tree->setBounds();

            bottom_right_tree = new Quad(capacity, depth + 1);
            bottom_right_tree->top_left = sf::Vector2i((bottom_right.x + top_left.x) / 2, (bottom_right.y + top_left.y) / 2);
            bottom_right_tree->bottom_right = bottom_right;
            bottom_right_tree->setBounds();

            for (Point* point : points)
            {
                top_left_tree->inQuad(point);
                top_right_tree->inQuad(point);
                bottom_left_tree->inQuad(point);
                bottom_right_tree->inQuad(point);
            }

            if (top_left_tree->points.size() > capacity)
                top_left_tree->subdivide();
            else
                top_left_tree->testCollisions();

            if (top_right_tree->points.size() > capacity)
                top_right_tree->subdivide();
            else
                top_right_tree->testCollisions();

            if (bottom_left_tree->points.size() > capacity)
                bottom_left_tree->subdivide();
            else
                bottom_left_tree->testCollisions();

            if (bottom_right_tree->points.size() > capacity)
                bottom_right_tree->subdivide();
            else
                bottom_right_tree->testCollisions();
        }

    }
    void update(std::vector<Point*> points)
    {
        delete top_left_tree;
        top_left_tree = NULL;
        delete top_right_tree;
        top_right_tree = NULL;
        delete bottom_left_tree;
        bottom_left_tree = NULL;
        delete bottom_right_tree;
        bottom_right_tree = NULL;
        if (points.size() > capacity)
            subdivide();
    }
    void inQuad(Point* point) 
    {
        if (point->x > top_left.x && point->x < bottom_right.x && point->y > top_left.y && point->y < bottom_right.y)
            points.push_back(point);
    }
    int getNumCollisions()
    {
        if (points.size() > capacity)
        {
            int val = 0;
            if (top_left_tree != NULL)
                val += top_left_tree->getNumCollisions();
            if (top_right_tree != NULL)
                val += top_right_tree->getNumCollisions();
            if (bottom_left_tree != NULL)
                val += bottom_left_tree->getNumCollisions();
            if (bottom_right_tree != NULL)
                val += bottom_right_tree->getNumCollisions();
            return val;
        }
        else
        {
            return points.size() * (points.size() - 1);
        }
    }
    void testCollisions() 
    {
        if (points.size() >= 2)
        {
            for (int i = 0; i < points.size() - 1; i++)
            {
                for (int j = i + 1; j < points.size(); j++)
                {
                    float x1 = points[i]->x;
                    float x2 = points[j]->x;
                    float y1 = points[i]->y;
                    float y2 = points[j]->y;
                    if ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) < 0.01)
                    {
                        points[i]->direction.x = -points[i]->direction.x;
                        points[i]->direction.y = -points[i]->direction.y;
                        points[j]->direction.x = -points[j]->direction.x;
                        points[j]->direction.y = -points[j]->direction.y;
                        float placeholder = points[i]->speed;
                        points[i]->speed = points[j]->speed;
                        points[j]->speed = points[i]->speed;
                    }
                }
            }
        }
    }
};

void showStats(float avg_fps, float fps, int num_points, int bucket_size, int num_collisions)
{
    std::cout << "Average FPS: " << avg_fps << std::endl;
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Number of Points: " << num_points << std::endl;
    std::cout << "Bucket Size: " << bucket_size << std::endl;
    std::cout << "Brute Force Collision Checks: " << num_points * (num_points - 1) << std::endl;
    std::cout << "Quadtree Collision Checks: " << num_collisions << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Quadtree Testing");

    Area area(100, 100);

    const int num_points = 15000;
    std::vector<Point*> points;
    for (int i = 0; i < num_points; i++)
    {   
        Point* point = new Point(rand() % window_width, rand() % window_height);
        points.push_back(point);
    }
    Quad* quad = new Quad(points, 2);

    bool toggle = false;
    bool toggle2 = true;
    bool toggle3 = false;
    bool toggle4 = false;

    sf::Clock update;

    sf::Clock dt;
    float average_fps = 0;
    float cur_fps = 0;
    int frames = 0;


    while (window.isOpen())
    {
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        area.update(mouse_pos);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::F)
                    showStats(average_fps, cur_fps, points.size(), quad->capacity, quad->getNumCollisions());
                if (event.key.code == sf::Keyboard::Num1)
                    quad->capacity++;
                if (event.key.code == sf::Keyboard::Num2)
                {
                    if (quad->capacity > 2)
                        quad->capacity--;
                }
                if (event.key.code == sf::Keyboard::R)
                    toggle = !toggle;
                if (event.key.code == sf::Keyboard::C)
                    toggle2 = !toggle2;
                if (event.key.code == sf::Keyboard::T)
                {
                    toggle3 = !toggle3;
                    if (!toggle3)
                    {
                        for (Point* point : points)
                            point->deselect();
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                toggle4 = true;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            if (toggle)
            {
                Point* point = new Point(mouse_pos.x, mouse_pos.y);
                points.push_back(point);
            } 
            else
            {
                Point* point = new Point(rand() % window_width, rand() % window_height);
                points.push_back(point);
            }
        }
        if (update.getElapsedTime().asSeconds() > 0.1)
        {
            for (Point* point : points)
                point->move(false);
            update.restart();
        }
        quad->update(points);

        // Selection
        if (toggle3)
        {
            if (toggle4)
            {
                for (Point* point : points)
                {
                    if (point->x > area.x1 && point->x < area.x2 && point->y > area.y1 && point->y < area.y2)
                        point->select();
                }
                toggle4 = false;
            }
        }

        /*
        // Brute Force collisions
        for (int i = 0; i < points.size() - 1; i++)
        {
            for (int j = i; j < points.size(); j++)
            {
                if (points[i]->x - points[j]->x < 0.001 && points[i]->y - points[j]->y < 0.001)
                {
                    points[i]->direction.x = -points[i]->direction.x;
                    points[i]->direction.y = -points[i]->direction.y;
                    points[j]->direction.x = -points[j]->direction.x;
                    points[j]->direction.y = -points[j]->direction.y;
                    float placeholder = points[i]->speed;
                    points[i]->speed = points[j]->speed;
                    points[j]->speed = points[i]->speed;
                }
            }
        }
        */

        window.clear();
        for (int i = 0; i < points.size(); i++)
            window.draw(*points[i]);
        if (toggle2)
            window.draw(*quad);
        if (toggle3)
            window.draw(area);
        window.display();

        cur_fps = 1 / dt.restart().asSeconds();
        average_fps = (average_fps * frames + cur_fps) / (frames + 1);
        frames++;
    }

    return 0;
}