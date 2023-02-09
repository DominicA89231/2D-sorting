#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#define window_width 500
#define window_height 500
#define PI 3.1415926535

class Point : public sf::Drawable
{
public:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(shape);
    }
    int speed;
    sf::CircleShape shape;
    sf::Vector2f direction;
    Point()
    {
        shape.setFillColor(sf::Color::Green);
        int radius = 5;
        shape.setRadius(radius);
        shape.setPosition(sf::Vector2f(rand() % (window_width - 2 * radius), rand() % (window_height - 2 * radius)));
        speed = 100 * (rand() % 9 + 1);
        float next_x = cos(rand() % 360 * PI / 180);
        float next_y = sin(rand() % 360 * PI / 180);
        float z = sqrt(next_x * next_x + next_y * next_y);
        direction = sf::Vector2f(next_x / z, next_y / z);
    }
    void move(float dt)
    {
        float next_x = speed * dt * direction.x;
        float next_y = speed * dt * direction.y;
        if (shape.getPosition().x + next_x < 0)
        {
            next_x = -shape.getPosition().x;
            direction.x = -direction.x;
        }
        if (shape.getPosition().x + next_x + 2 * shape.getRadius() > window_width)
        {
            next_x = window_width - (shape.getPosition().x + 2 * shape.getRadius());
            direction.x = -direction.x;
        }
        if (shape.getPosition().y + next_y < 0)
        {
            next_y = -shape.getPosition().y;
            direction.y = -direction.y;
        }
        if (shape.getPosition().y + next_y + 2 * shape.getRadius() > window_width)
        {
            next_y = window_width - (shape.getPosition().y + 2 * shape.getRadius());
            direction.y = -direction.y;
        }
        shape.move(sf::Vector2f(next_x, next_y));
    }
    void select()
    {
        shape.setFillColor(sf::Color::Blue);
    }
    void deselect()
    {
        shape.setFillColor(sf::Color::Green);
    }
};

class Section
{
public:
    std::vector<Point*> points;
    int id;
    void insert(Point* point)
    {
        points.push_back(point);
    }
    void remove(Point* point)
    {
        for (int i = 0; i < points.size(); i++)
        {
            if (points[i] == point)
            {
                points.erase(points.begin() + i);
            }
        }
    }
    void testCollisions()
    {
        if (points.size() >= 2)
        {
            for (int i = 0; i < points.size() - 1; i++)
            {
                Point* point1 = points[i];
                for (int j = i + 1; j < points.size(); j++)
                {
                    Point* point2 = points[j];
                    float x = (point1->shape.getPosition().x + point1->shape.getRadius()) - (point2->shape.getPosition().x + point2->shape.getRadius());
                    float y = (point1->shape.getPosition().y + point1->shape.getRadius()) - (point2->shape.getPosition().y + point2->shape.getRadius());
                    if (sqrt(x * x + y * y) < point1->shape.getRadius() + point2->shape.getRadius())
                    {
                        point1->direction = -point1->direction;
                        point2->direction = -point2->direction;
                        float placeholder = point1->speed;
                        point1->speed = point2->speed;
                        point2->speed = placeholder;
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
    Grid(int w, int h, std::vector<Point*> points)
    {
        grid_width = w;
        grid_height = h;
        num_rows = window_height / w;
        num_cols = window_width / h;
        sections.resize((num_rows + 1) * (num_cols + 1));
        for (int i = 0; i < sections.size(); i++)
            sections[i] = new Section();
        for (Point* point : points)
            insert(point);
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
    void remove(Point* point)
    {
        float x1 = point->shape.getPosition().x;
        float x2 = point->shape.getPosition().x + 2 * point->shape.getRadius();
        float y1 = point->shape.getPosition().y;
        float y2 = point->shape.getPosition().y + 2 * point->shape.getRadius();
        int top_left = floor(x1 / grid_width) + (num_cols + 1) * floor(y1 / grid_height);
        sections[top_left]->remove(point);
        int top_right = (int)x2 / grid_width + (num_cols + 1) * floor(y1 / grid_height);
        if (top_left != top_right)
            sections[top_right]->remove(point);
        int bottom_left = floor(x1 / grid_width) + (num_cols + 1) * floor(y2 / grid_height);
        if (top_left != bottom_left)
            sections[bottom_left]->remove(point);
        int bottom_right = floor(x2 / grid_width) + (num_cols + 1) * floor(y2 / grid_height);
        if (bottom_right != bottom_left && bottom_right != top_right)
            sections[bottom_right]->remove(point);
    }
    void insert(Point* point)
    {
        float x1 = point->shape.getPosition().x;
        float x2 = point->shape.getPosition().x + 2 * point->shape.getRadius();
        float y1 = point->shape.getPosition().y;
        float y2 = point->shape.getPosition().y + 2 * point->shape.getRadius();
        int top_left = floor(x1 / grid_width) + (num_cols + 1) * floor(y1 / grid_height);
        sections[top_left]->insert(point);
        int top_right = floor(x2 / grid_width) + (num_cols + 1) * floor(y1 / grid_height);
        if (top_left != top_right)
            sections[top_right]->insert(point);
        int bottom_left = floor(x1 / grid_width) + (num_cols + 1) * floor(y2 / grid_height);
        if (top_left != bottom_left)
            sections[bottom_left]->insert(point);
        int bottom_right = floor(x2 / grid_width) + (num_cols + 1) * floor(y2 / grid_height);
        if (bottom_right != bottom_left && bottom_right != top_right)
            sections[bottom_right]->insert(point);
    }
    void testCollisions()
    {
        for (Section* section : sections)
        {
            section->testCollisions();
        }
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "SFML works!");
    int num_points = 30;
    std::vector<Point*> points;
    for (int i = 0; i < num_points; i++)
    {
        Point* point = new Point();
        points.push_back(point);
    }
    Grid* grid = new Grid(50, 50, points);

    sf::Clock update;
    sf::Clock clock;
    float dt = 0;
    bool paused = false;

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
                    paused = !paused;
            }
        }

        if (update.getElapsedTime().asSeconds() > 0.001 && !paused)
        {
            for (Point* point : points)
            {
                // point->deselect();
                grid->remove(point);
                point->move(dt);
                grid->insert(point);
            }
            grid->testCollisions();
            update.restart();
        }

        window.clear();
        for (Point* point : points)
            window.draw(*point);
        window.draw(*grid);
        window.display();
        dt = clock.restart().asSeconds();
        if (1 / dt < 200)
            std::cout << 1 / dt << std::endl;
    }

    return 0;
}