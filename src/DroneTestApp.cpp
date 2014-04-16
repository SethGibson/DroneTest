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
	//void updateHand(PXCGesture::GeoNode::Label pHand, vector<Vec3f> &pHandData);
	//void updateOSC();
	void onButtonOscDn();
	void onButtonCamDn();
	void draw();
	void quit();

private:
	vector<Vec3f> mHand0Pos;
	vector<Vec3f> mHand1Pos;
	gl::Texture mBlobTex;
	PXCHand mHand0;
	PXCHand mHand1;

	params::InterfaceGl mGUI;
	int32_t mParamPort;
	string mParamIp, mParamOscStatus, mParamCamStatus;
	bool mParamOscOnOff, mParamCamOnOff, mParamFps60;

	osc::Sender mSender;

	UtilPipeline mPXC;
	PXCGesture *mGesture;
};
void DroneTestApp::prepareSettings(Settings *pSettings)
{
	pSettings->setWindowSize(900,510);
	pSettings->setFrameRate(30);
}

void DroneTestApp::setup()
{
	setupGUI();
	setupPXC();
	//setupOSC();
}

void DroneTestApp::setupGUI()
{
	mParamPort = 2710;
	mParamIp = "192.168.1.255";
	mParamOscOnOff = false;
	mParamCamOnOff = false;
	mParamFps60 = false;
	mParamOscStatus = "Not Sending";
	mParamCamStatus = "Disabled";
	
	mGUI = params::InterfaceGl("Hand Position OSC", Vec2i(200,300), ColorA(0.05f,0.4f,0.65f,0.5f));
	mGUI.setOptions("", "valueswidth=80 position='20 15'");	

	mGUI.addSeparator();
	mGUI.addParam("60 Fps", &mParamFps60);
	mGUI.addButton("Start/Stop Camera", std::bind(&DroneTestApp::onButtonCamDn, this)); 
	mGUI.addParam("Camera Status: ", &mParamCamStatus);
	mGUI.addSeparator();
	mGUI.addParam("Ip", &mParamIp);
	mGUI.addParam("Port", &mParamPort);
	mGUI.addButton("Start/Stop OSC", std::bind(&DroneTestApp::onButtonOscDn, this)); 
	mGUI.addParam("OSC Status: ", &mParamOscStatus);
	mGUI.addSeparator();
}

void DroneTestApp::setupPXC()
{
	mPXC.EnableGesture();
	mPXC.Init();
}

void DroneTestApp::setupOSC()
{
	mSender.setup(mParamIp, mParamPort, true);
}

void DroneTestApp::mouseDown( MouseEvent event )
{
}

void DroneTestApp::update()
{
	if(mParamCamOnOff)
	{
		updateCamera();
		/*
		if(mSending)
			updateOSC();
		*/
	}
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
		//updateHand(PXCGesture::GeoNode::LABEL_BODY_HAND_PRIMARY, mHand0);	
		//updateHand(PXCGesture::GeoNode::LABEL_BODY_HAND_SECONDARY, mHand1);	
		mPXC.ReleaseFrame();
	}
}
/*
void DroneTestApp::updateHand(PXCGesture::GeoNode::Label pHand, vector<Vec3f> &pHandData)
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
*/

/*
void DroneTestApp::updateOSC()
{
	osc::Bundle handBundle;

	if(mHand0.IsActive())
	{
		osc::Message hand0;
		hand0.setAddress("/hand0/");
		hand0.addIntArg((int)mHand0.HasPalm);
		hand0.addFloatArg(mHand0.PalmNode.ImagePos.x);palm.addFloatArg(mHand0.PalmNode.ImagePos.y);
		hand0.addFloatArg(mHand0.PalmNode.WorldPos.x);palm.addFloatArg(mHand0.PalmNode.WorldPos.y);palm.addFloatArg(mHand0.PalmNode.WorldPos.z);
		hand0.setRemoteEndpoint(mIpParam, mPortParam);
		handBundle.addMessage(hand0);
	}

	if(mHand1.IsActive())
	{
		osc::Message hand1;
		hand1.setAddress("/hand1/");
		hand1.addIntArg((int)mHand1.HasPalm);
		hand1.addFloatArg(mHand1.PalmNode.ImagePos.x);palm.addFloatArg(mHand1.PalmNode.ImagePos.y);
		hand1.addFloatArg(mHand1.PalmNode.WorldPos.x);palm.addFloatArg(mHand1.PalmNode.WorldPos.y);palm.addFloatArg(mHand1.PalmNode.WorldPos.z);
		hand1.setRemoteEndpoint(mIpParam, mPortParam);
		handBundle.addMessage(hand1);
	}

	mSender.sendBundle(handBundle);
}
*/
void DroneTestApp::onButtonOscDn()
{
	mParamOscOnOff=!mParamOscOnOff;
	mParamOscStatus = mParamOscOnOff?"Sending":"Not Sending";
	if(mParamOscOnOff)
		mSender.setup(mParamIp, mParamPort, true);
}

void DroneTestApp::onButtonCamDn()
{
	mParamCamOnOff=!mParamCamOnOff;
	mParamCamStatus = mParamOscOnOff?"Enabled":"Disabled";
	if(mParamCamOnOff)
	{
		mPXC = UtilPipeline();
		pxcU32 cFps = mParamFps60?60:30;
		PXCSizeU32 cDepthSize;cDepthSize.width=320;cDepthSize.height=240;
		mPXC.EnableImage(PXCImage::COLOR_FORMAT_DEPTH);
		mPXC.QueryCapture()->SetFilter(PXCImage::IMAGE_TYPE_DEPTH, cDepthSize, cFps);
		mPXC.Init();
	}
	else
		mPXC.Close();
}

void DroneTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

	mGUI.draw();
	gl::pushMatrices();
	gl::translate(Vec2f(240,15));
	if(mParamCamOnOff)
	{
		gl::color(Color::white());
		gl::draw(mBlobTex,Rectf(0,0,640,480));
	}
	else
	{
		gl::color(Color(1,0,0));
		gl::drawStrokedRect(Rectf(0,0,640,480));
	}
	gl::popMatrices();
}

void DroneTestApp::quit()
{
	mPXC.Close();
}


CINDER_APP_NATIVE( DroneTestApp, RendererGl )
