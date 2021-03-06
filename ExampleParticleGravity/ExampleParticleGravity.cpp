// ExampleParticleGravity.cpp : Definiert den Einstiegspunkt f�r die Konsolenanwendung.
//

#include "stdafx.h"
#include "BBE/BrotBoxEngine.h"

class Particle;

static bbe::Random random;
static bbe::List<Particle> particles;

static float size = 1;

class Particle
{
public:
	bbe::Vector3 pos;
	bbe::Vector3 speed;
	bbe::IcoSphere m_sphere;

	Particle()
	{
		pos = random.randomVector3InUnitSphere() * 100.0f;
	}

	Particle(const bbe::Vector3 &pos)
		: pos(pos)
	{
		//do nothing
	}

	void updateSpeed(float timeSinceLastFrame)
	{
		for (Particle &p : particles)
		{
			if (this == &p)
			{
				continue;
			}
			bbe::Vector3 dir = p.pos - pos;
			float length = dir.getLength();
			if (length != 0)
			{
				bbe::Vector3 gravity = dir.normalize() / length / length / 10;
				bbe::Vector3 antiTouch = dir.normalize() / length / length / length / 10;
				speed = speed + gravity - antiTouch;
			}
		}

		speed = speed * 0.99f;
	}

	void updatePos(float timeSinceLastFrame)
	{

		pos = pos + speed * timeSinceLastFrame;

		m_sphere.set(pos, bbe::Vector3(size), bbe::Vector3(1), 0);
	}

	void draw(bbe::PrimitiveBrush3D &brush)
	{
		brush.fillIcoSphere(m_sphere);
	}
};


class MyGame : public bbe::Game
{

	bbe::CameraControlNoClip ccnc = bbe::CameraControlNoClip(this);
	bbe::PointLight light;

	float maxSpeed = 0;
	float minSpeed = 0;

	// Geerbt �ber Game
	virtual void onStart() override
	{
		light.setFalloffMode(bbe::LightFalloffMode::LIGHT_FALLOFF_NONE);
		light.setLightStrength(1);
		light.setPosition(bbe::Vector3(100, 100, 100));
		for (int i = 0; i < 100; i++)
		{
			particles.add(Particle());
		}
	}
	virtual void update(float timeSinceLastFrame) override
	{
		ccnc.update(timeSinceLastFrame);
		std::cout << (1 / timeSinceLastFrame) << std::endl;

		if (isMousePressed(bbe::MouseButton::LEFT))
		{
			particles.add(Particle(ccnc.getCameraPos()));
		}

		timeSinceLastFrame = 0.016f;

		if (isKeyPressed(bbe::Key::T))
		{
			size += 0.1f;
		}

		if (isKeyPressed(bbe::Key::G))
		{
			size -= 0.1f;
		}

		if (isKeyPressed(bbe::Key::Z))
		{
			for (Particle &p : particles)
			{
				p.speed = bbe::Vector3();
			}
		}


		for (int iterations = 0; iterations < 20; iterations++)
		{
			for (Particle &p : particles)
			{
				p.updateSpeed(timeSinceLastFrame);
			}
			for (Particle &p : particles)
			{
				p.updatePos(timeSinceLastFrame);
			}
		}

		minSpeed = 100000.0f;
		maxSpeed = 0;
		for (int i = 0; i < particles.getLength(); i++)
		{
			float speed = particles[i].speed.getLength();
			if (speed > maxSpeed) maxSpeed = speed;
			if (speed < minSpeed) minSpeed = speed;
		}
	}
	virtual void draw3D(bbe::PrimitiveBrush3D & brush) override
	{
		brush.setCamera(ccnc.getCameraPos(), ccnc.getCameraTarget());
		for (Particle &p : particles)
		{
			float speed = p.speed.getLength();
			float percentage = (speed - minSpeed) / (maxSpeed + minSpeed);
			brush.setColor(1, 1 - percentage, 1 - percentage);
			p.draw(brush);
		}
	}
	virtual void draw2D(bbe::PrimitiveBrush2D & brush) override
	{
	}
	virtual void onEnd() override
	{
	}
};

int main()
{
	MyGame game;
	game.start(1280, 720, "Particle Gravity");
    return 0;
}

