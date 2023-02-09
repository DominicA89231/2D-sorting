#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>

#define PI 3.1415926535
#define window_width 1024
#define window_height 1024

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
        point->color = sf::Color::White;
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
        if (bot_left_tree != NULL)
            bot_left_tree->draw(target, states);
        if (bot_right_tree != NULL)
            bot_right_tree->draw(target, states);
    }
    std::vector<Point*> points;
    int capacity, depth;
    sf::Vector2i top_left, bot_right;
    sf::VertexArray bounds;
    Quad* top_left_tree;
    Quad* top_right_tree;
    Quad* bot_left_tree;
    Quad* bot_right_tree;
    Quad()
    {
        top_left = sf::Vector2i(0, 0);
        bot_right = sf::Vector2i(window_width, window_height);
        capacity = 10;
        depth = 1;
        top_left_tree = NULL;
        top_right_tree = NULL;
        bot_left_tree = NULL;
        bot_right_tree = NULL;
        setBounds();
    }
    Quad(int x1, int y1, int x2, int y2, int _depth)
    {
        top_left = sf::Vector2i(x1, y1);
        bot_right = sf::Vector2i(x2, y2);
        capacity = 10;
        depth = _depth;
        top_left_tree = NULL;
        top_right_tree = NULL;
        bot_left_tree = NULL;
        bot_right_tree = NULL;
        setBounds();

    }
    void subdivide()
    {
        int midx = (top_left.x + bot_right.x) / 2;
        int midy = (top_left.y + bot_right.y) / 2;
        top_left_tree = new Quad(top_left.x, top_left.y, midx, midy, depth + 1);
        top_right_tree = new Quad(midx, top_left.y, bot_right.x, midy, depth + 1);
        bot_left_tree = new Quad(top_left.x, midy, midx, bot_right.y, depth + 1);
        bot_right_tree = new Quad(midx, midy, bot_right.x, bot_right.y, depth + 1);

        for (Point* point : points)
        {
            if (top_left_tree->inBoundary(point))
                top_left_tree->points.push_back(point);
            if (top_right_tree->inBoundary(point))
                top_right_tree->points.push_back(point);
            if (bot_left_tree->inBoundary(point))
                bot_left_tree->points.push_back(point);
            if (bot_right_tree->inBoundary(point))
                bot_right_tree->points.push_back(point);
        }
    }
    bool inBoundary(Point* point)
    {
        return point->x > top_left.x && point->x < bot_right.x&& point->y > top_left.y && point->y < bot_right.y;
    }
    void insert(Point* point)
    {
        if (inBoundary(point))
        {
            points.push_back(point);
            if (top_left_tree != NULL)
            {
                top_left_tree->insert(point);
                top_right_tree->insert(point);
                bot_left_tree->insert(point);
                bot_right_tree->insert(point);
            }
            else
            {
                if (points.size() > capacity)
                {
                    if (depth < 6)
                    {
                        subdivide();
                        top_left_tree->insert(point);
                        top_right_tree->insert(point);
                        bot_left_tree->insert(point);
                        bot_right_tree->insert(point);
                    }
                }
            }
        }
    }
    void remove(Point* point)
    {
        for (int i = 0; i < points.size(); i++)
        {
            if (points[i] == point)
                points.erase(points.begin() + i);
        }

        if (top_left_tree != NULL)
        {
            if (points.size() <= capacity)
            {
                top_left_tree = NULL;
                top_right_tree = NULL;
                bot_left_tree = NULL;
                bot_right_tree = NULL;
                return;
            }
            top_left_tree->remove(point);
            top_right_tree->remove(point);
            bot_left_tree->remove(point);
            bot_right_tree->remove(point);
        }
    }
    void collide()
    {
        if (top_left_tree != NULL)
        {
            top_left_tree->collide();
            top_right_tree->collide();
            bot_left_tree->collide();
            bot_right_tree->collide();
        }
        else
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
    }
    void setBounds()
    {
        bounds.resize(8);
        bounds.setPrimitiveType(sf::Lines);
        bounds[0].position = sf::Vector2f(top_left.x, top_left.y);
        bounds[1].position = sf::Vector2f(bot_right.x, top_left.y);
        bounds[2].position = sf::Vector2f(bot_right.x, bot_right.y);
        bounds[3].position = sf::Vector2f(top_left.x, bot_right.y);
        bounds[6].position = sf::Vector2f(top_left.x, top_left.y);
        bounds[4].position = sf::Vector2f(bot_right.x, top_left.y);
        bounds[5].position = sf::Vector2f(bot_right.x, bot_right.y);
        bounds[7].position = sf::Vector2f(top_left.x, bot_right.y);
    }
};

void showStats(float avg_fps, float fps, int num_points, int bucket_size)
{
    std::cout << "Min FPS: " << avg_fps << std::endl;
    std::cout << "FPS: " << fps << std::endl;
    std::cout << "Number of Points: " << num_points << std::endl;
    std::cout << "Bucket Size: " << bucket_size << std::endl;
    std::cout << "Brute Force Collision Checks: " << num_points * (num_points - 1) << std::endl;
    // std::cout << "Quadtree Collision Checks: " << num_collisions << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Quadtree Testing");

    Area area(100, 100);
    Quad* quad = new Quad();

    const int num_points = 1000;
    std::vector<Point*> points;
    for (int i = 0; i < num_points; i++)
    {   
        Point* point = new Point(rand() % window_width, rand() % window_height);
        points.push_back(point);
        if (i % 50 == 0)
            point->select();
    }

    bool toggle = false;
    bool toggle2 = true;
    bool toggle3 = false;

    sf::Clock update;

    sf::Clock dt;
    float min_fps = 10000;
    float cur_fps = 0;


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
                    showStats(min_fps, cur_fps, points.size(), quad->capacity);
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
            {
                quad->remove(point);
                point->move(false);
                quad->insert(point);
            }
            update.restart();
        }
        quad->collide();

        // Selection
        if (toggle3)
        {
            for (Point* point : points)
            {
                if (point->x > area.x1 && point->x < area.x2 && point->y > area.y1 && point->y < area.y2)
                    point->select();
                else
                    point->deselect();
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
        min_fps = std::min(cur_fps, min_fps);
    }

    return 0;
}