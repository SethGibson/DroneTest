#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/params/Params.h"
#include "OscSender.h"
#include "util_pipeline.h"
#include "PXCHand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DroneTestApp : public AppNative {
public:
	void prepareSettings(Settings *pSettings);
	void setup();
	void setupGUI();
	void setupPXC();
	void setupOSC();
	void mouseDown( MouseEvent event );	
	void update();
	void updateCamera();
	void updateHand(PXCGesture::GeoNode::Label pHand, PXCHand &pHandData);
	void updateOSC();
	void onButtonDn();
	void draw();


private:
	gl::Texture mBlobTex;
	PXCHand mHand0;
	PXCHand mHand1;

	params::InterfaceGl mGUI;
	int32_t mPortParam;
	string mIpParam, mStatusParam;
	bool mSending;

	osc::Sender mSender;

	UtilPipeline mPXC;
	PXCGesture *mGesture;
};
void DroneTestApp::prepareSettings(Settings *pSettings)
{
	pSettings->setWindowSize(568,272);
	pSettings->setFrameRate(30);
}

void DroneTestApp::setup()
{
	setupGUI();
	setupPXC();
	setupOSC();
}

void DroneTestApp::setupGUI()
{
	mPortParam = 2710;
	mIpParam = "192.168.1.255";
	mSending = false;
	mStatusParam = "Not Sending";
	
	mGUI = params::InterfaceGl("OSC Test", Vec2i(200,240), ColorA(0.05f,0.4f,0.65f,0.5f));
	mGUI.setOptions("", "valueswidth=110");	

	mGUI.addSeparator();
	mGUI.addParam("Ip", &mIpParam);
	mGUI.addParam("Port", &mPortParam);
	mGUI.addSeparator();
	mGUI.addButton("Start/Stop", std::bind(&DroneTestApp::onButtonDn, this)); 
	mGUI.addSeparator();
	mGUI.addParam("Status: ", &mStatusParam);
}

void DroneTestApp::setupPXC()
{
	mPXC.EnableImage(PXCImage::COLOR_FORMAT_DEPTH);
	mPXC.EnableGesture();
	mPXC.Init();
}

void DroneTestApp::setupOSC()
{
	mSender.setup(mIpParam, mPortParam, true);
}

void DroneTestApp::mouseDown( MouseEvent event )
{
}

void DroneTestApp::update()
{
	updateCamera();
	if(mSending)
		updateOSC();
}

void DroneTestApp::updateCamera()
{
	if(mPXC.AcquireFrame(true))
	{
		mGesture = mPXC.QueryGesture();
		PXCImage *cBlob;
		if(mGesture->QueryBlobImage(PXCGesture::Blob::LABEL_SCENE,0,&cBlob)>=PXC_STATUS_NO_ERROR)
		{
			PXCImage::ImageData cBlobData;
			if(cBlob->AcquireAccess(PXCImage::ACCESS_READ, &cBlobData)>=PXC_STATUS_NO_ERROR)
			{
				mBlobTex = gl::Texture(cBlobData.planes[0],GL_LUMINANCE,320,240);
				cBlob->ReleaseAccess(&cBlobData);
			}
		}
		updateHand(PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, mHand0);	
		updateHand(PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY, mHand1);	
		mPXC.ReleaseFrame();
	}
}

void DroneTestApp::updateHand(PXCGesture::GeoNode::Label pHand, PXCHand &pHandData)
{

	PXCGesture::GeoNode cHandNode;
	pHandData.clear();

	if(mGesture->QueryNodeData(0, pHand, &cHandNode)>=PXC_STATUS_NO_ERROR)	{
		pHandData.setNode(0,Vec2f(cHandNode.positionImage.x, cHandNode.positionImage.y), Vec3f(cHandNode.positionWorld.x,cHandNode.positionWorld.z, cHandNode.positionWorld.y));
		pHandData.HasPalm = true;	}

	if(mGesture->QueryNodeData(0, pHand|PXCGesture::GeoNode::LABEL_FINGER_THUMB, &cHandNode)>=PXC_STATUS_NO_ERROR)	{
		pHandData.setNode(1,Vec2f(cHandNode.positionImage.x, cHandNode.positionImage.y), Vec3f(cHandNode.positionWorld.x,cHandNode.positionWorld.z, cHandNode.positionWorld.y));
		pHandData.HasThumb = true;	}

	if(mGesture->QueryNodeData(0, pHand|PXCGesture::GeoNode::LABEL_FINGER_INDEX, &cHandNode)>=PXC_STATUS_NO_ERROR)	{
		pHandData.setNode(2,Vec2f(cHandNode.positionImage.x, cHandNode.positionImage.y), Vec3f(cHandNode.positionWorld.x,cHandNode.positionWorld.z, cHandNode.positionWorld.y));
		pHandData.HasIndex = true;	}

	if(mGesture->QueryNodeData(0, pHand|PXCGesture::GeoNode::LABEL_FINGER_MIDDLE, &cHandNode)>=PXC_STATUS_NO_ERROR)	{
		pHandData.setNode(3,Vec2f(cHandNode.positionImage.x, cHandNode.positionImage.y), Vec3f(cHandNode.positionWorld.x,cHandNode.positionWorld.z, cHandNode.positionWorld.y));
		pHandData.HasMiddle = true;	}

	if(mGesture->QueryNodeData(0, pHand|PXCGesture::GeoNode::LABEL_FINGER_RING, &cHandNode)>=PXC_STATUS_NO_ERROR)	{
		pHandData.setNode(4,Vec2f(cHandNode.positionImage.x, cHandNode.positionImage.y), Vec3f(cHandNode.positionWorld.x,cHandNode.positionWorld.z, cHandNode.positionWorld.y));
		pHandData.HasRing = true;	}

	if(mGesture->QueryNodeData(0, pHand|PXCGesture::GeoNode::LABEL_FINGER_PINKY, &cHandNode)>=PXC_STATUS_NO_ERROR)	{
		pHandData.setNode(5,Vec2f(cHandNode.positionImage.x, cHandNode.positionImage.y), Vec3f(cHandNode.positionWorld.x,cHandNode.positionWorld.z, cHandNode.positionWorld.y));
		pHandData.HasPinky = true;	}
}

void DroneTestApp::updateOSC()
{
	osc::Bundle hand0Bundle, hand1Bundle;
	if(mHand0.IsActive())
	{
		osc::Message palm, thumb, index, middle, ring, pinky;
		palm.setAddress("/hand0/palm/");
		palm.addIntArg((int)mHand0.HasPalm);
		palm.addFloatArg(mHand0.PalmNode.ImagePos.x);palm.addFloatArg(mHand0.PalmNode.ImagePos.y);
		palm.addFloatArg(mHand0.PalmNode.WorldPos.x);palm.addFloatArg(mHand0.PalmNode.WorldPos.y);palm.addFloatArg(mHand0.PalmNode.WorldPos.z);
		palm.setRemoteEndpoint(mIpParam, mPortParam);
		hand0Bundle.addMessage(palm);

		thumb.setAddress("/hand0/thumb/");
		thumb.addIntArg((int)mHand0.HasThumb);
		thumb.addFloatArg(mHand0.ThumbNode.ImagePos.x);thumb.addFloatArg(mHand0.ThumbNode.ImagePos.y);
		thumb.addFloatArg(mHand0.ThumbNode.WorldPos.x);thumb.addFloatArg(mHand0.ThumbNode.WorldPos.y);thumb.addFloatArg(mHand0.ThumbNode.WorldPos.z);
		thumb.setRemoteEndpoint(mIpParam, mPortParam);
		hand0Bundle.addMessage(thumb);

		index.setAddress("/hand0/index/");
		index.addIntArg((int)mHand0.HasIndex);
		index.addFloatArg(mHand0.IndexNode.ImagePos.x);index.addFloatArg(mHand0.IndexNode.ImagePos.y);
		index.addFloatArg(mHand0.IndexNode.WorldPos.x);index.addFloatArg(mHand0.IndexNode.WorldPos.y);index.addFloatArg(mHand0.IndexNode.WorldPos.z);
		index.setRemoteEndpoint(mIpParam, mPortParam);
		hand0Bundle.addMessage(index);

		middle.setAddress("/hand0/middle/");
		middle.addIntArg((int)mHand0.HasMiddle);
		middle.addFloatArg(mHand0.MiddleNode.ImagePos.x);middle.addFloatArg(mHand0.MiddleNode.ImagePos.y);
		middle.addFloatArg(mHand0.MiddleNode.WorldPos.x);middle.addFloatArg(mHand0.MiddleNode.WorldPos.y);middle.addFloatArg(mHand0.MiddleNode.WorldPos.z);
		middle.setRemoteEndpoint(mIpParam, mPortParam);
		hand0Bundle.addMessage(middle);

		ring.setAddress("/hand0/ring/");
		ring.addIntArg((int)mHand0.HasRing);
		ring.addFloatArg(mHand0.RingNode.ImagePos.x);ring.addFloatArg(mHand0.RingNode.ImagePos.y);
		ring.addFloatArg(mHand0.RingNode.WorldPos.x);ring.addFloatArg(mHand0.RingNode.WorldPos.y);ring.addFloatArg(mHand0.RingNode.WorldPos.z);
		ring.setRemoteEndpoint(mIpParam, mPortParam);
		hand0Bundle.addMessage(ring);

		pinky.setAddress("/hand0/pinky/");
		pinky.addIntArg((int)mHand0.HasPinky);
		pinky.addFloatArg(mHand0.PinkyNode.ImagePos.x);pinky.addFloatArg(mHand0.PinkyNode.ImagePos.y);
		pinky.addFloatArg(mHand0.PinkyNode.WorldPos.x);pinky.addFloatArg(mHand0.PinkyNode.WorldPos.y);pinky.addFloatArg(mHand0.PinkyNode.WorldPos.z);
		pinky.setRemoteEndpoint(mIpParam, mPortParam);
		hand0Bundle.addMessage(pinky);
	}

	if(mHand1.IsActive())
	{
		osc::Message palm, thumb, index, middle, ring, pinky;
		palm.setAddress("/hand1/palm/");
		palm.addIntArg((int)mHand1.HasPalm);
		palm.addFloatArg(mHand1.PalmNode.ImagePos.x);palm.addFloatArg(mHand1.PalmNode.ImagePos.y);
		palm.addFloatArg(mHand1.PalmNode.WorldPos.x);palm.addFloatArg(mHand1.PalmNode.WorldPos.y);palm.addFloatArg(mHand1.PalmNode.WorldPos.z);
		palm.setRemoteEndpoint(mIpParam, mPortParam);
		hand1Bundle.addMessage(palm);

		thumb.setAddress("/hand1/thumb/");
		thumb.addIntArg((int)mHand1.HasThumb);
		thumb.addFloatArg(mHand1.ThumbNode.ImagePos.x);thumb.addFloatArg(mHand1.ThumbNode.ImagePos.y);
		thumb.addFloatArg(mHand1.ThumbNode.WorldPos.x);thumb.addFloatArg(mHand1.ThumbNode.WorldPos.y);thumb.addFloatArg(mHand1.ThumbNode.WorldPos.z);
		thumb.setRemoteEndpoint(mIpParam, mPortParam);
		hand1Bundle.addMessage(thumb);

		index.setAddress("/hand1/index/");
		index.addIntArg((int)mHand1.HasIndex);
		index.addFloatArg(mHand1.IndexNode.ImagePos.x);index.addFloatArg(mHand1.IndexNode.ImagePos.y);
		index.addFloatArg(mHand1.IndexNode.WorldPos.x);index.addFloatArg(mHand1.IndexNode.WorldPos.y);index.addFloatArg(mHand1.IndexNode.WorldPos.z);
		index.setRemoteEndpoint(mIpParam, mPortParam);
		hand1Bundle.addMessage(index);

		middle.setAddress("/hand1/middle/");
		middle.addIntArg((int)mHand1.HasMiddle);
		middle.addFloatArg(mHand1.MiddleNode.ImagePos.x);middle.addFloatArg(mHand1.MiddleNode.ImagePos.y);
		middle.addFloatArg(mHand1.MiddleNode.WorldPos.x);middle.addFloatArg(mHand1.MiddleNode.WorldPos.y);middle.addFloatArg(mHand1.MiddleNode.WorldPos.z);
		middle.setRemoteEndpoint(mIpParam, mPortParam);
		hand1Bundle.addMessage(middle);

		ring.setAddress("/hand1/ring/");
		ring.addIntArg((int)mHand1.HasRing);
		ring.addFloatArg(mHand1.RingNode.ImagePos.x);ring.addFloatArg(mHand1.RingNode.ImagePos.y);
		ring.addFloatArg(mHand1.RingNode.WorldPos.x);ring.addFloatArg(mHand1.RingNode.WorldPos.y);ring.addFloatArg(mHand1.RingNode.WorldPos.z);
		ring.setRemoteEndpoint(mIpParam, mPortParam);
		hand1Bundle.addMessage(ring);

		pinky.setAddress("/hand1/pinky/");
		pinky.addIntArg((int)mHand1.HasPinky);
		pinky.addFloatArg(mHand1.PinkyNode.ImagePos.x);pinky.addFloatArg(mHand1.PinkyNode.ImagePos.y);
		pinky.addFloatArg(mHand1.PinkyNode.WorldPos.x);pinky.addFloatArg(mHand1.PinkyNode.WorldPos.y);pinky.addFloatArg(mHand1.PinkyNode.WorldPos.z);
		pinky.setRemoteEndpoint(mIpParam, mPortParam);
		hand1Bundle.addMessage(pinky);
	}

	mSender.sendBundle(hand0Bundle);
	mSender.sendBundle(hand1Bundle);
}

void DroneTestApp::onButtonDn()
{
	mSending=!mSending;
	mStatusParam = mSending?"Sending":"Not Sending";
	if(mSending)
		mSender.setup(mIpParam, mPortParam, true);
}

void DroneTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

	mGUI.draw();
	gl::pushMatrices();
	gl::translate(Vec2f(232,16));
	gl::draw(mBlobTex,Vec2f::zero());
	mHand0.draw(1.0f,Vec2f::zero());
	mHand1.draw(1.0f,Vec2f::zero());
	gl::popMatrices();
}

CINDER_APP_NATIVE( DroneTestApp, RendererGl )
