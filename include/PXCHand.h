#ifndef __PXCHAND_H__
#define __PXCHAND_H__
#include "cinder/app/AppNative.h"

using namespace ci;
// Some utility structs ==============================================================
struct PXCNode
{
	Vec2f ImagePos;
	Vec3f WorldPos;
	void setPos(Vec2f pPos){ImagePos.set(pPos);};
	void setPos(Vec3f pPos){WorldPos.set(pPos);};
};

struct PXCHand
{
	bool HasPalm, HasThumb, HasIndex, HasMiddle, HasRing, HasPinky;
	PXCNode PalmNode, ThumbNode, IndexNode, MiddleNode, RingNode, PinkyNode;
	bool IsActive(){return HasPalm||HasThumb||HasIndex||HasMiddle||HasRing||HasPinky;};
	void clear(){HasPalm=HasThumb=HasIndex=HasMiddle=HasRing=HasPinky=false;};
	void setNode(int pId, Vec2f pImagePos, Vec3f pWorldPos);
	void draw(float pScale, Vec2f pOffset);
};

void PXCHand::setNode(int pId, Vec2f pImagePos, Vec3f pWorldPos)
{
	switch(pId)
	{
		case 0:	{ PalmNode.setPos(pImagePos);PalmNode.setPos(pWorldPos); break; }
		case 1:	{ ThumbNode.setPos(pImagePos);ThumbNode.setPos(pWorldPos); break; }
		case 2:	{ IndexNode.setPos(pImagePos);IndexNode.setPos(pWorldPos); break; }
		case 3:	{ MiddleNode.setPos(pImagePos);MiddleNode.setPos(pWorldPos); break; }
		case 4:	{ RingNode.setPos(pImagePos);RingNode.setPos(pWorldPos); break; }
		case 5:	{ PinkyNode.setPos(pImagePos);PinkyNode.setPos(pWorldPos); break; }
	}
}

void PXCHand::draw(float pScale, Vec2f pOffset)
{
	if(IsActive())
	{
		gl::enableAlphaBlending();
		gl::lineWidth(2);
		gl::pushMatrices();
		gl::translate(pOffset);

		//draw Palm
		if(HasPalm)
		{
			gl::color(ColorA(0,1,0,0.25f));
			gl::drawSolidCircle(Vec2f(PalmNode.ImagePos.x*pScale, PalmNode.ImagePos.y*pScale), 10);
			gl::color(ColorA(0,1,0,1));
			gl::drawStrokedCircle(Vec2f(PalmNode.ImagePos.x*pScale, PalmNode.ImagePos.y*pScale), 10);
		}

		//draw Thumb
		if(HasThumb)
		{
			gl::color(ColorA(1,0,0,1));
			gl::drawStrokedCircle(Vec2f(ThumbNode.ImagePos.x*pScale, ThumbNode.ImagePos.y*pScale), 5);
		}

		//draw Index
		if(HasIndex)
		{
			gl::color(ColorA(0,0,1,1));
			gl::drawStrokedCircle(Vec2f(IndexNode.ImagePos.x*pScale, IndexNode.ImagePos.y*pScale), 5);
		}

		//draw Middle
		if(HasMiddle)
		{
			gl::color(ColorA(1,0,1,1));
			gl::drawStrokedCircle(Vec2f(MiddleNode.ImagePos.x*pScale, MiddleNode.ImagePos.y*pScale), 5);
		}

		//draw Ring
		if(HasRing)
		{
			gl::color(ColorA(1,1,0,1));
			gl::drawStrokedCircle(Vec2f(RingNode.ImagePos.x*pScale, RingNode.ImagePos.y*pScale), 5);
		}

		//draw Pinky
		if(HasPinky)
		{
			gl::color(ColorA(0,1,1,1));
			gl::drawStrokedCircle(Vec2f(PinkyNode.ImagePos.x*pScale, PinkyNode.ImagePos.y*pScale), 5);
		}
		gl::popMatrices();
		gl::color(Color::white());
		gl::lineWidth(1);
		gl::disableAlphaBlending();
	}
}

#endif