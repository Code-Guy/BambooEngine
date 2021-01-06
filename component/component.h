#pragma once

class Component
{
public:
	virtual void pre();
	virtual void begin();
	virtual void tick(float deltaTime);
	virtual void end();
	virtual void post();

private:

};