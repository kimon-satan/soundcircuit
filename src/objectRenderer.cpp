/*
 *  objectRenderer.cpp
 *  soundCircuit
 *
 *  Created by Simon Katan on 05/01/2012.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "objectRenderer.h"

objectRenderer::objectRenderer(){

	isTrackData = false;
	isNodeData = false;
	isBlipData = false;

}

void objectRenderer::draw(ofRectangle t_view){
	
	viewPort = t_view;
	
	drawTracks();
	drawNodes();
	drawBlips();
	
}


void objectRenderer::drawTracks(){
	
	for(int i = 0; i < tracks->size(); i ++){
		
		drawTrack(&tracks->at(i));
	}
	
	if(isPreview){
		
		for(int i = 0; i <2; i++){
			if(previewTracks[i].getIsValid())drawTrack(&previewTracks[i]);
		}
	}

}

void objectRenderer::drawTrack(segment * t){
	
	glPushMatrix();
	//glTranslatef(0, 0, -2);
	
	ofVec2f sp(t->getStartPos());
	ofVec2f lp;
	
	if(t->getDirection().x > 0){
		lp.set(t->getTestArea().x + t->getTestArea().width + 2, sp.y);
	}else{
		lp.set(sp.x, t->getTestArea().y + t->getTestArea().height + 2);
	}
	
	if(checkIsVisible(sp, lp, t->getDirection())){
		ofSetColor(100);
		ofLine(sp.x,sp.y,lp.x,lp.y);
	}
	
	if(t->getIsWrapped()){
		
		ofVec2f wsp;
		ofVec2f wlp;
		
		if(t->getDirection().x > 0){
			wsp.set(t->getWrapTestArea().x - 1, sp.y);
			wlp.set(t->getWrapTestArea().x + t->getWrapTestArea().width + 1, sp.y);
		}else{
			wsp.set(sp.x,t->getWrapTestArea().y - 1);
			wlp.set(sp.x, t->getWrapTestArea().y + t->getWrapTestArea().height + 1);
		}
		
		if(checkIsVisible(wsp, wlp, t->getDirection())){
			ofSetColor(100);
			ofLine(wsp.x, wsp.y, wlp.x, wlp.y);
		}
		
	}

	
	if(t->getIsSelected()){
		ofEnableAlphaBlending();
		ofSetColor(255, 0, 0, 100);
		ofFill();
		ofCircle(t->getSelectPos(), 5);
		ofDisableAlphaBlending();
	}
	
	if(isTrackData){
		ofDrawBitmapString("sp", sp);
		ofNoFill();
		ofSetColor(255, 0, 0);
		ofRect(t->getTestArea());
		
		if(t->getIsWrapped()){
			ofSetColor(0, 0, 255);
			ofRect(t->getWrapTestArea());
		}
	}
	
	glPopMatrix();


}

bool objectRenderer::checkIsVisible(ofVec2f a, ofVec2f b, ofVec2f t_dir){
	
	if(t_dir.x > 0){
		
		if(viewPort.inside(viewPort.x + viewPort.width/2, a.y)){
			if(a.x < viewPort.x + viewPort.width && b.x > viewPort.x)return true;
		}
		
	}else if(t_dir.y > 0){
		
		if(viewPort.inside(a.x, viewPort.y + viewPort.height/2)){
			if(a.y < viewPort.y + viewPort.height && b.y > viewPort.y)return true;
		}
		
	}
	
	return false;
}


bool objectRenderer::checkIsVisible(ofRectangle t){

	vector<ofVec2f> points;
	points.push_back(ofVec2f(t.x,t.y));
	points.push_back(ofVec2f(t.x + t.width,t.y));
	points.push_back(ofVec2f(t.x + t.width,t.y + t.height));
	points.push_back(ofVec2f(t.x,t.y + t.height));
	
	for(int i = 0; i < points.size(); i++){
		points[i].x = min(points[i].x, world_dims.x/2);
		points[i].x = max(points[i].x, -world_dims.x/2);
		points[i].y = min(points[i].y, world_dims.y/2);
		points[i].y = max(points[i].y, -world_dims.y/2);
		if(viewPort.inside(points[i]))return true;
	}
	
	return false;
}

void objectRenderer::drawNodes(){
	
	for(vector<node>::iterator it = nodes->begin(); it != nodes->end(); it++){
		
		ofVec2f pos(it->getPos());
		
		if(viewPort.inside(pos)){
			
			if(it->getIsSelected()){
				ofEnableAlphaBlending();
				ofFill();
				ofSetColor(255, 0, 0, 100);
				ofCircle(pos, 5);
				ofDisableAlphaBlending();
				
			}
			
			if(isNodeData){
				ofNoFill();
				ofSetColor(0, 0, 255);
				ofCircle(pos, 5);
				ofDrawBitmapString(ofToString(it->getIndex()), pos -3);
			}
		}
	}
	

	
}

void objectRenderer::drawBlips(){

	glPushMatrix();
	
	for(vector<blip>::iterator it = blips->begin(); it != blips->end(); it++){
		
		if(isBlipData){
			ofSetColor(255,0,0);
			ofDrawBitmapString("sp", it->getStartPos());
			ofNoFill();
			ofSetColor(0, 100, 255);
			ofRect(it->getTestArea());
			
			if(it->getIsWrapped()){
				ofSetColor(0, 255, 0);
				ofRect(it->getWrapTestArea());
			}
		}else{
			//this still isn't quite correct as doesn't handle wrap in both directions (needs a wrapXDraw and WrapYDraw) and corresponding methods
			
			if(!it->getDrawer()->getIsDrawWrap()){
				if(checkIsVisible(it->getDrawer()->getDrawRect()))it->draw();
			}else{
				
				if(checkIsVisible(it->getDrawer()->getDrawRect())){
					it->draw();
				}
				if(checkIsVisible(it->getDrawer()->getWrapDrawRect())){
					it->draw(true);
				}
			}
		}
	}
	
	if(previewBlip.getIsValid()){
	
		if(isBlipData){
			ofSetColor(255,0,0);
			ofDrawBitmapString("sp", previewBlip.getStartPos());
			ofNoFill();
			ofSetColor(0, 100, 255);
			ofRect(previewBlip.getTestArea());
			
			if(previewBlip.getIsWrapped()){
				ofSetColor(0, 255, 0);
				ofRect(previewBlip.getWrapTestArea());
			}
		}else {
			previewBlip.draw();
		}

		
	}
	
	glPopMatrix();

}



//getters and setters

void objectRenderer::toggleTrackData(){isTrackData = !isTrackData;}
void objectRenderer::toggleNodeData(){isNodeData = !isNodeData;}
void objectRenderer::toggleBlipData(){isBlipData = !isBlipData;}