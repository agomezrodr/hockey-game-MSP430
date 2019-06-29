// Adrian Gomez Rodriguez
// Computer Architecture 1
// T-Th 7:30-8:50
// Prof. Dr. Freudenthal
// TA. Daniel Cervantes 

#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include "buzzer.h"
#define GREEN_LED BIT6

Region fence = {{10,20}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}};
AbRect rect = {abRectGetBounds, abRectCheck, {2,10}};
u_char scoreP1 = '0';
u_char scoreP2 = '0';

static int state = 0;

//background of game
  AbRectOutline fieldOutline = {
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2-5, screenHeight/2-15}
};

//background edge of the field
  Layer fieldLayer = {
  (AbShape *)&fieldOutline, {screenWidth/2, screenHeight/2},
  {0,0}, {0,0}, COLOR_BLACK,
};

// Pock
  Layer layer3 = {       
  (AbShape *)&circle5, {(screenWidth/2), (screenHeight/2)}, 
  {0,0}, {0,0},COLOR_BLACK,&fieldLayer,
};

// Pusher pock1
  Layer layer1 = {
  (AbShape *) &circle10, {screenWidth/2-50, screenHeight/2+5},    
  {0,0}, {0,0}, COLOR_RED, &layer3
};

//Pusher pock2
  Layer layer2 = {    
  (AbShape *)&circle10, {screenWidth/2+50, screenHeight/2+5}, 
  {0,0}, {0,0}, COLOR_RED, &layer1,
};

//Line of middle circle
  Layer layer4 = {
  (AbShape *)&circle16, {screenWidth/2, screenHeight/2}, 
  {0,0}, {0,0}, COLOR_WHITE, &layer2,
};

//circle inside circle to make circunference
  Layer layer5 = {
  (AbShape *)&circle14, {screenWidth/2, screenHeight/2}, 
  {0,0}, {0,0}, COLOR_BLACK, &layer4,
};

  typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
}

MovLayer;
MovLayer moveLayer1 = { &layer1, {0,10}, 0 }; 
MovLayer moveLayer3 = { &layer3, {1, 5}, 0 }; 
MovLayer moveLayer2 = { &layer2, {0,10}, 0 }; 


void movLayerDraw(MovLayer *movLayers, Layer *layers){
  int row;
  int col;
  MovLayer *movLayer;
  and_sr(~8);       // disable interrupts  
  
  // for each moving layer
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { 
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);        //disable interrupts 
  
  //moving layer
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { 
    Region area;
    layerGetBounds(movLayer->layer, &area);                   
    lcd_setArea(area.topLeft.axes[0],                        
                area.topLeft.axes[1], 
                area.botRight.axes[0], 
                area.botRight.axes[1]);
    for (row = area.topLeft.axes[1]; row <= area.botRight.axes[1]; row++) {
      for (col = area.topLeft.axes[0]; col <= area.botRight.axes[0]; col++) {
            Vec2 pixelPos = {col, row};     //Find the coordinate
            u_int color = bgColor;
            Layer *putLayer;    
	for (putLayer = layers; putLayer; putLayer = putLayer->next) { // put all layers in order 
	  if (abShapeCheck(putLayer->abShape, &putLayer->pos, &pixelPos)) {  //keep running the layers with the movement
	    color = putLayer->color;
	    break; 
	  }  
	} 
	lcd_writeColor(color); 
      }
    }
  } 
}  

//Move the pock
void movePock(MovLayer *moveLayer, Region *fence, MovLayer *moveLayer2, MovLayer *moveLayer3){
  Vec2 newPosition;
  u_char axis;
  Region shapeArea;
  int velocity;
  for (; moveLayer; moveLayer = moveLayer->next) {
    vec2Add(&newPosition, &moveLayer->layer->posNext, &moveLayer->velocity);
    abShapeGetBounds(moveLayer->layer->abShape, &newPosition, &shapeArea);
    for (axis = 0; axis < 2; axis ++){
      if((shapeArea.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||  //Top left
	  (shapeArea.botRight.axes[axis] > fence->botRight.axes[axis]) ||   //Bottom Rigth
	  (abShapeCheck(moveLayer3->layer->abShape, &moveLayer3->layer->posNext, &moveLayer->layer->posNext)) || //layer 3
	  (abShapeCheck(moveLayer2->layer->abShape, &moveLayer2->layer->posNext, &moveLayer->layer->posNext))){   //layer 2
          
      velocity = moveLayer->velocity.axes[axis] = -moveLayer->velocity.axes[axis];  
      newPosition.axes[axis] += (2*velocity);   //speed
    } 
      else if((shapeArea.topLeft.axes[0] < fence->topLeft.axes[0])){  //player 2
	  newPosition.axes[0] = screenWidth/2;
	  newPosition.axes[1] = screenHeight/2;
	  scoreP2 = scoreP2 - 255;
    } 
      else if((shapeArea.botRight.axes[0] > fence->botRight.axes[0])){  //player1
	  newPosition.axes[0] = screenWidth/2;
	  newPosition.axes[1] = screenHeight/2;
	  scoreP1 = scoreP1 - 255;
    }
      if(scoreP1 == '7' || scoreP2 == '7'){   // check the score
	  state = 1;
    }
  } 
    moveLayer->layer->posNext = newPosition;  //new position of the pock
 } 
}

void moveUp(MovLayer *moveLayer, Region *fence){
  Vec2 newPosition;
  u_char axis;
  Region shapeArea;
  for (; moveLayer; moveLayer = moveLayer->next) {
    vec2Sub(&newPosition, &moveLayer->layer->posNext, &moveLayer->velocity);
    abShapeGetBounds(moveLayer->layer->abShape, &newPosition, &shapeArea);
    for (axis = 1; axis < 2; axis ++) {
      if ((shapeArea.topLeft.axes[axis] < fence->topLeft.axes[axis]) || //handles if collision happens in the fence
	  (shapeArea.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
	int velocity = moveLayer->velocity.axes[axis];
	newPosition.axes[axis] += (velocity);   //speed
      }
    }
    moveLayer->layer->posNext = newPosition;  //new position of the pock
  } 
}

void moveDown(MovLayer *moveLayer, Region *fence){
  Vec2 newPosition;
  u_char axis;
  Region shapeArea;
  for (; moveLayer; moveLayer = moveLayer->next) {
    vec2Add(&newPosition, &moveLayer->layer->posNext, &moveLayer->velocity);
    abShapeGetBounds(moveLayer->layer->abShape, &newPosition, &shapeArea);
    for (axis = 1; axis < 2; axis ++) {
      if ((shapeArea.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||  //handles if collision happens in the fence
	  (shapeArea.botRight.axes[axis] > fence->botRight.axes[axis]) ) {   
	int velocity = -moveLayer->velocity.axes[axis];     
	newPosition.axes[axis] += (2*velocity);        //speed
      }
    } 
    moveLayer->layer->posNext = newPosition;  //new position of the pock
  }
}


u_int bgColor = COLOR_WHITE;     // The background color 
int redrawScreen = 1;           // redrawn screen
Region fieldEdge;              // fence 

void main(){
  
 P1DIR |= GREEN_LED;//Green led on when game on
 P1OUT |= GREEN_LED;

  configureClocks(); //initialize clocks
  lcd_init(); //initialize lcd
  buzzer_init(); //initialize buzzer
  p2sw_init(15); //initialize switches has to be 15 if not crash

  layerInit(&layer2); 
  layerDraw(&layer2); 
  //layerInit(&layer4); 
  //layerDraw(&layer4); 
  //layerInit(&layer5);       //draw center
  //layerDraw(&layer5); 
  layerGetBounds(&fieldLayer, &fieldEdge);
  enableWDTInterrupts();  
  or_sr(0x8);
  u_int switches = p2sw_read();

  for(;;){ 
    while (!redrawScreen) { // freez the game until press button on green board
      P1OUT &= ~GREEN_LED; // Turn iff green led
        or_sr(0x10); //< CPU OFF
    }
    P1OUT |= GREEN_LED; // Turn on green led during game
    redrawScreen = 0;
 
    movLayerDraw(&moveLayer3, &layer2);
    movLayerDraw(&moveLayer2, &layer2);
    movLayerDraw(&moveLayer1, &layer2); 
    
    // Print all string on board
    drawString5x7(15, 5, "Ducks     Sharks", COLOR_CHOCOLATE, COLOR_WHITE);
    drawString5x7(53, 5, "NHL", COLOR_BLACK, COLOR_WHITE);
    drawString5x7(7, 20, "Make 7 goals to win", COLOR_DARK_GREEN, COLOR_WHITE);
    drawChar5x7(5, 5, scoreP1, COLOR_GOLD, COLOR_RED); 
    drawChar5x7(115, 5, scoreP2, COLOR_GOLD, COLOR_BLUE);
    drawString5x7(15, 150, "By Adrian Gomez R", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(62, 140, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 130, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 120, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 110, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 100, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 80, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 70, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 48, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 38, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 28, "|", COLOR_RED, COLOR_WHITE);
    drawString5x7(62, 56, "H", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(69, 61, "O", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(76, 68, "C", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(76, 79, "K", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(69, 85, "E", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(62, 89, "Y", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(55, 85, "G", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(48, 79, "A", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(48, 68, "M", COLOR_NAVY, COLOR_WHITE);
    drawString5x7(55, 61, "E", COLOR_NAVY, COLOR_WHITE);
  }
}

// Watchdog timer interrupt handler.
void wdt_c_handler(){
  static short count = 0;
  P1OUT |= GREEN_LED;      //Green LED on when game on 
  count ++;
  u_int switches = p2sw_read();

  if(count == 12){       
    //switch statement to check if they win      
    switch(state){
    case 0:
      movePock(&moveLayer3, &fieldEdge, &moveLayer1, &moveLayer2);  
      break;
    case 1:
      layerDraw(&layer2);
      if(scoreP1 > scoreP2)
	drawString5x7(40, 50, "DUCKS WIN!!", COLOR_BLACK, COLOR_WHITE);
      else if(scoreP1 < scoreP2)
	drawString5x7(40, 50, "SHARKS WIN!!", COLOR_BLACK, COLOR_WHITE);
      break;
    }
    // Play music during game
    Music();
    
    //Move push pocket1
    if(switches & (1<<3)){
      moveUp(&moveLayer2, &fieldEdge);
    }
    if(switches & (1<<2)){
      moveDown(&moveLayer2, &fieldEdge);
    }
    //Move push pocket2
    if(switches & (1<<1)){
      moveUp(&moveLayer1, &fieldEdge);
    }
    if(switches & (1<<0)){
      moveDown(&moveLayer1, &fieldEdge);
    }
    redrawScreen = 1;
    count = 0;
  }
  P1OUT &= ~GREEN_LED;    
}
