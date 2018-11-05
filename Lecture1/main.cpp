#include "Canvas2D.h"
#include "RGBColors.h"
#include <iostream>

//TODO: option to draw bounding box

namespace jm
{
	Canvas2D my_canvas("This is my digital canvas!", 1024, 768, false, 2); // Canvas : (-1.0, -1.0) x (1.0, 1.0)

	class MyTank
	{
	public:
		vec3 center = vec3(0.0f, 0.0f, 0.0f);
		vec3 direction = vec3(1.0f, 0.0f, 0.0f); //TODO: implement vec2

		void draw()
		{
			my_canvas.beginTransformation();
			my_canvas.translate(center.x, center.y);
			my_canvas.drawFilledBox(RGBColors::green, 0.25f, 0.1f);
			my_canvas.translate(-0.02f, 0.1f);
			my_canvas.drawFilledBox(RGBColors::blue, 0.15f, 0.09f);
			my_canvas.translate(0.15f, 0.0f);
			my_canvas.drawFilledBox(RGBColors::red, 0.15f, 0.03f);
			my_canvas.endTransformation();
		}
	};

	class MyBullet
	{
	public:
		vec3 center = vec3(0.0f, 0.0f, 0.0f);
		vec3 velocity = vec3(0.0f, 0.0f, 0.0f);

		void draw()
		{
			my_canvas.beginTransformation();
			my_canvas.translate(center.x, center.y);
			my_canvas.drawFilledCircle(RGBColors::yellow, 0.02f, 8);
			my_canvas.endTransformation();
		}

		void update(const float& dt)
		{
			center += velocity * dt;
		}

	};
}

using namespace jm;
using namespace std;

int main(void)
{
	float time = 0.0;

	MyTank tank;

	MyBullet *bullet = nullptr;
	//TODO: allow multiple bullets
	//TODO: delete bullets when they go out of the screen

	bool space_key_pressed = false;

	// Lambda function
	// Let's not use Lambda, use polymorphism
	my_canvas.show([&]
	{
		if (my_canvas.isKeyPressed(GLFW_KEY_ESCAPE)) {
			cout << "Terminating program" << endl;
			exit(0);
		}

		my_canvas.drawLine(RGBColors::red, vec2(0.0f, 0.0f), RGBColors::blue, vec2(1.0f, 1.0f));

		// move tank
		if (my_canvas.isKeyPressed(GLFW_KEY_LEFT))	tank.center.x -= 0.01f;
		if (my_canvas.isKeyPressed(GLFW_KEY_RIGHT))	tank.center.x += 0.01f;
		if (my_canvas.isKeyPressed(GLFW_KEY_UP))	tank.center.y += 0.01f;
		if (my_canvas.isKeyPressed(GLFW_KEY_DOWN))	tank.center.y -= 0.01f;

		// shoot a cannon ball
		if (my_canvas.isKeyPressed(GLFW_KEY_SPACE))	space_key_pressed = true;

		if (space_key_pressed && my_canvas.isKeyReleased(GLFW_KEY_SPACE))
		{
			if (bullet != nullptr) delete bullet;
			bullet = new MyBullet;
			bullet->center = tank.center;
			bullet->center.x += 0.2f;
			bullet->center.y += 0.1f;
			bullet->velocity = vec3(2.0f, 0.0f, 0.0f);

			space_key_pressed = false;
		}

		if (bullet != nullptr) bullet->update(1 / 60.0f);

		// rendering
		tank.draw();
		if (bullet != nullptr) bullet->draw();

		time += 1 / 60.0f;

		// sleep 
	}
	);

	return 0;
}
