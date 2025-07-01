#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "atlas.ppm" //adds Atlas[]

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 640

#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2

#define RAD 0.0174533

#define TARGET_FPS 40

#define MAP_WIDTH 8
#define MAP_HEIGHT 8
#define CELL_SIZE 64

#define WALL_MOD 1.0
const double epsilon=0.0001;
#define VERT_SIZE 7

#define FLOOR_MOD_R 0.3
#define FLOOR_MOD_G 0.3
#define FLOOR_MOD_B 0.3

#define CEILING_MOD_R 0.8
#define CEILING_MOD_G 0.8
#define CEILING_MOD_B 0.8

#define RAY_OFFSET 1.0
#define RAY_COUNT 60.0
float ray_depths[static_cast<int>(RAY_COUNT)]={0.0};
#define FOV RAY_COUNT*RAY_OFFSET

//(rays width/2)/tan(fov/2)
#define FLOOR_COEF 158*2 //Ev this

#define SPRITE_MAX_SCALE 120

//TODO make this OOP, make double fixAngle(double prev_angle)
//if int(px)/CELL_SIZE==winposx and y too then win.
//if expanding map don't check dof to 8 check it to the highest width or height

double difference(double p1, double p2){
	return fabs(p1-p2);
}
int map[]={
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,2,0,0,0,1,
	1,0,0,3,0,0,0,1,
	1,0,1,1,1,4,3,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1
};

int mapFloors[]={
	2,3,2,1,2,2,2,2,
	2,3,2,1,2,2,2,2,
	2,3,2,1,2,2,2,2,
	2,3,2,1,2,2,2,2,
	2,3,2,1,2,2,2,2,
	2,3,2,3,2,2,2,2,
	2,3,2,3,2,2,2,2,
	2,3,2,3,2,2,2,2
};

int mapCeilings[]={
	2,2,2,2,2,2,2,2,
	2,0,0,2,2,2,2,2,
	2,0,0,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2
};

class Renderer {
	private:
		sf::Texture sky;
		sf::Sprite sky_sprite;
	public:
		sf::RenderWindow& window;

		// Constructor to initialize with an existing window
		Renderer(sf::RenderWindow& win) : window(win) {
			
		}

		void init() {
			window.setFramerateLimit(TARGET_FPS);
			if (!sky.loadFromFile("bg1.png")) {
				perror("Couldn't load texture \"bg1.png\".");
				return;
			} else {
				sky_sprite.setTexture(sky);
				sky_sprite.setPosition(0,0);
				sky_sprite.setScale(2.0,2.0);
			}
		}
		void draw_sky(){
			window.draw(sky_sprite);
		}
};

class Player {
	public:
		double posx;
		double posy;
		double dx;
		double dy;
		double angle;
		double speed;
		sf::CircleShape shape;
		
		int a_down;
		int s_down;
		int d_down;
		int w_down;
		
		int size;

		// Constructor
		Player(double x, double y) : posx(x), posy(y), shape(10) { // Use initializer list for CircleShape
			angle=0.06;
			dx=0.0;
			dy=0.0;
			w_down=0;
			a_down=0;
			s_down=0;
			d_down=0;
			size=30;
			shape.setFillColor(sf::Color(100, 250, 50)); // Set initial color
			speed=5.0;
			std::cout << "Player Init!" << "\n";
		}

		void draw(Renderer& renderer) {
			shape.setPosition(static_cast<int>(posx), static_cast<int>(posy));
			renderer.window.draw(shape);
		}

		void update_input() {
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				a_down=1;
			} else {
				a_down=0;
			}
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				d_down=1;
			} else {
				d_down=0;
			}
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				
				w_down=1;
			} else {
				w_down=0;
			}
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				s_down=1;
			} else {
				s_down=0;
			}
			
			if(a_down==1){
				angle-=0.1;
				if(angle<0){
					angle+=2*PI;
				}
				dx=cos(angle)*speed;
				dy=sin(angle)*speed;
			}
			
			if(d_down==1){
				angle+=0.1;
				if(angle>2*PI){
					angle-=2*PI;
				}
				dx=cos(angle)*speed;
				dy=sin(angle)*speed;
			}
			
			int xo=size;
			if(dx<0){
				xo=-size;
			}
			int ipx=posx/CELL_SIZE;
			int ipx_add_xo=(posx+xo)/CELL_SIZE;
			int ipx_sub_xo=(posx-xo)/CELL_SIZE;
			
			int yo=size;
			if(dy<0){
				yo=-size;
			}
			int ipy=posy/CELL_SIZE;
			int ipy_add_yo=(posy+yo)/CELL_SIZE;
			int ipy_sub_yo=(posy-yo)/CELL_SIZE;
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
				if(map[ipy_add_yo*MAP_WIDTH+ipx_add_xo]==4){ //Door
					map[ipy_add_yo*MAP_WIDTH+ipx_add_xo]=0;
				}
			}
			/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
				FLOOR_COEF++;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
				FLOOR_COEF--;
			}*/
			
			
			if(w_down==1){
				if(map[ipy*MAP_WIDTH + ipx_add_xo]==0){
					posx+=dx;
				}
				if(map[ipy_add_yo*MAP_WIDTH+ipx]==0){
					posy+=dy;
				}
			}
			
			if(s_down==1){
				if(map[ipy*MAP_WIDTH + ipx_sub_xo]==0){
					posx-=dx;
				}
				if(map[ipy_sub_yo*MAP_WIDTH+ipx]==0){
					posy-=dy;
				}
			}
			
		}
};

class Sprite {
	private:
		double dx;
		double dy;
		int tile_x;
		int tile_y;
	public:
		int type; //Enemy,? key? TODO abstract this behaviour
		int state; //On / oFF
		int texture_map; //texture
		int size;
		double tx,ty,tz; //pos
		
		double speed;
		sf::CircleShape shape;
		
		// Constructor
		Sprite(double x, double y,double z) : tx(x*CELL_SIZE), ty(y*CELL_SIZE*-1), tz(z), shape(20) {
			state=1;
			
			//Type specifics
			speed=0.04;
			type=1;
			size=20;
			
			shape.setFillColor(sf::Color(100, 250, 50));
		}
		void update(Player& player){
			//enemy attack
			if(difference(player.posx,tx)<size&&difference(player.posy,ty)<size){
				state=0;
			}
			/*if(player.posx>tx){
				dx=speed*TARGET_FPS;
			} else {
				dx=-speed*TARGET_FPS;
			}
			
			if(player.posy>ty){
				dy=speed*TARGET_FPS;
			} else {
				dy=-speed*TARGET_FPS;
			}
			
			tile_x = static_cast<int>((tx + dx) / CELL_SIZE);
			tile_y = static_cast<int>((ty + dy) / CELL_SIZE);
		
			if (map[static_cast<int>(ty) * MAP_WIDTH + tile_x] != 0) {  // 0 is walkable
				tx += dx;
			}
			if (map[tile_y * MAP_WIDTH + static_cast<int>(tx)] != 0) {  // 0 is walkable
				ty += dy;
			}*/
			
		}
		void drawSpriteToPlayerPersp(Renderer& renderer,Player& player){
			if(state==0){
				//else update
				return;
			}
			float sy=ty+player.posx; //temps
			float sx=tx-player.posy;
			float sz=tz;
			
			//rotate around origin:
			float CS=sin(player.angle);
			float SN=cos(player.angle);
			float a=sy*CS+sx*SN;
			float Z_depth=sx*CS-sy*SN;
			sx=a;
			sy=Z_depth;
			
			if (sy <= 0.0f) return;
			
			sx=(sx*(SCREEN_HEIGHT/2.0)/Z_depth)+(SCREEN_WIDTH/2); //to screenspace
			sy=(sz*(SCREEN_HEIGHT/2.0)/Z_depth)+(SCREEN_HEIGHT/2);
			
			if(sx>0&&sx<SCREEN_WIDTH && Z_depth<ray_depths[static_cast<int>(sx/SCREEN_WIDTH*RAY_COUNT)]){
				int scale=32*80/Z_depth;   //scale sprite based on distance
				if(scale<0){
					scale=0;
				}else if(scale>SPRITE_MAX_SCALE){
					scale=SPRITE_MAX_SCALE;
				}  
			
				shape.setPosition(static_cast<int>(sx),static_cast<int>(sy));
				renderer.window.draw(shape);
				
				//texture
			}	
			
		}
};

double dist(double ax,double ay,double bx, double by,double ang){
	return (sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay)));
}

bool draw2d=false;
void drawRays3D(Renderer renderer,Player player){
	int mx;
	int my;
	int mp;
	int dof;
	
	double rx;
	double ry;
	double ra;
	double xo=0.0;
	double yo=0.0;
	double distT;
	
	int wall_type=0;
	int wall_type2=0;
	
	ra=player.angle-RAD*RAY_COUNT;
	if(ra<0){
		ra+=2*PI;
	}
	if(ra>2*PI){
		ra-=2*PI;
	}
	for(int r=0;r<RAY_COUNT*2;r++){
		dof=0;
		double disH=100000.0,hx=player.posx,hy=player.posy;
		double aTan=-1/tan(ra);
		if(ra>PI){ //up or down?
			ry=((static_cast<int>(player.posy)/CELL_SIZE)*CELL_SIZE)-epsilon;
			rx=(player.posy-ry)*aTan+player.posx;
			yo=-CELL_SIZE;
			xo=-yo*aTan;
		}
		if(ra<PI){ //up or down?
			ry=((static_cast<int>(player.posy)/CELL_SIZE)*CELL_SIZE)+CELL_SIZE;
			rx=(player.posy-ry)*aTan+player.posx;
			yo=CELL_SIZE;
			xo=-yo*aTan;
		}
		if(ra==PI||ra==0){ //looking straight left or right, impossible for hitting horizontal line
			rx=player.posx;
			ry=player.posy;
			dof=8;
		}
		
		//perform depth check
		while(dof<8){
			mx=static_cast<int>(rx)/CELL_SIZE;
			my=static_cast<int>(ry)/CELL_SIZE; //by 64 division.
			
			
			mp=my*MAP_WIDTH+mx; //cell the player is in.
			if (mp>0 && mp<MAP_HEIGHT*MAP_WIDTH){
				
				if(map[mp]>0){
					hx=rx;
					hy=ry;
					disH=dist(player.posx,player.posy,hx,hy,ra);
					dof=8;
					wall_type=map[mp]-1;
				} else {
					rx+=xo;
					ry+=yo;
				}
			} else {
				rx+=xo;
				ry+=yo;
				
			}
			dof++;
			
		}
		
		//---vertical lines---
		
		dof=0;
		double disV=100000.0,vx=player.posx,vy=player.posy;
		double nTan=-tan(ra);
		if(ra>P2 && ra<P3){
			rx=((static_cast<int>(player.posx)/CELL_SIZE)*CELL_SIZE)-epsilon;
			ry=(player.posx-rx)*nTan+player.posy;
			xo=-CELL_SIZE;
			yo=-xo*nTan;
		}
		if(ra<P2 || ra>P3){
			rx=((static_cast<int>(player.posx)/CELL_SIZE)*CELL_SIZE)+CELL_SIZE;
			ry=(player.posx-rx)*nTan+player.posy;
			xo=CELL_SIZE;
			yo=-xo*nTan;
		}
		if(ra==PI||ra==0){  //up or down?
			rx=player.posx;
			ry=player.posy;
			dof=8;
		}
		
		//perform depth check
		while(dof<8){
			mx=static_cast<int>(rx)/CELL_SIZE;
			my=static_cast<int>(ry)/CELL_SIZE; //by 64 division.
			
			mp=my*MAP_WIDTH+mx; //cell the player is in.
			if (mp>0 && mp<MAP_HEIGHT*MAP_WIDTH){
				
				if(map[mp]>0){
					vx=rx;
					vy=ry;
					disV=dist(player.posx,player.posy,vx,vy,ra);
					
					dof=8;
					wall_type2=map[mp]-1;
				} else {
					rx+=xo;
					ry+=yo;
				}
			} else {
				rx+=xo;
				ry+=yo;
				
			}
			dof++;
			
		}
		
		int is_h=0;
		double light_strength=1.0;
		if(disV<disH){
			rx=vx;
			ry=vy;
			distT=disV;
			wall_type=wall_type2;
			//vertical hit
		} else {
			rx=hx;
			ry=hy;
			distT=disH;
			is_h=1;
			//horizontal hit
			light_strength=0.7;
		}
		if(draw2d==true){
			sf::Vertex line[2];
			line[0].position = sf::Vector2f(static_cast<int>(player.posx), static_cast<int>(player.posy));
			line[0].color = sf::Color::Green;
			line[1].position = sf::Vector2f(static_cast<int>(rx), static_cast<int>(ry));
			line[1].color = sf::Color::Green;
			renderer.window.draw(line, 2, sf::Lines);
		}
		
		ray_depths[r]=distT;
		//-- draw 3d walls
		double ca=player.angle-ra;
		if(ca<0){
			ca+=2*PI;
		}
		if(ca>2*PI){
			ca-=2*PI;
		}
		distT*=cos(ca);
		
		
		int lineH=(CELL_SIZE*SCREEN_HEIGHT)/distT;
		double ty_step=32.0/static_cast<double>(lineH);
		double ty_off=0.0;
		double lineOffset=SCREEN_HEIGHT/2.0-(lineH>>1);
		if(lineH>SCREEN_HEIGHT){
			ty_off=(lineH-SCREEN_HEIGHT)/2.0;
			lineH=SCREEN_HEIGHT;
		}
		
		int y;
		double ty=ty_off*ty_step;
		double tx=0.0;
		if(is_h==1){
			tx=static_cast<int>(rx/2.0)%32;
			if(ra>PI){
				tx=31-tx;
			}
		} else {
			tx=static_cast<int>(ry/2.0)%32;
			if(ra>P2&&ra<P3){
				tx=31-tx;
			}
		}
		ty+=wall_type*32; //Next texture
		
		
		
		sf::VertexArray walls(sf::Quads);
		sf::VertexArray floors(sf::Quads);
		sf::VertexArray ceilings(sf::Quads);

		int pixel,red,green,blue;
		// Draw walls
		for (y = 0; y < lineH; y++) {
			pixel=(static_cast<int>(ty) * 32 + static_cast<int>(tx))*3+(wall_type*32*32*3);
			red=Atlas[pixel]*light_strength;
			green=Atlas[pixel+1]*light_strength;
			blue=Atlas[pixel+2]*light_strength;
			sf::Color color(red,green,blue);

			// Add a quad for each rectangle
			sf::Vector2f topLeft(r * VERT_SIZE, y + lineOffset + ty_off);
			sf::Vector2f topRight(r * VERT_SIZE + VERT_SIZE, y + lineOffset + ty_off);
			sf::Vector2f bottomLeft(r * VERT_SIZE, y + lineOffset + ty_off + 1);
			sf::Vector2f bottomRight(r * VERT_SIZE + VERT_SIZE, y + lineOffset + ty_off + 1);

			walls.append(sf::Vertex(topLeft, color));
			walls.append(sf::Vertex(topRight, color));
			walls.append(sf::Vertex(bottomRight, color));
			walls.append(sf::Vertex(bottomLeft, color));

			ty += ty_step;
		}
		
		// Draw floors and ceilings
		for (y = lineH + lineOffset + ty_off; y < SCREEN_HEIGHT; y++) {
			double dy = y - (SCREEN_HEIGHT / 2 -1);
			double t_rad = player.angle - ra;

			if (t_rad < 0){
				t_rad += 2 * PI;
			}
			if (t_rad > 2 * PI){
				t_rad -= 2 * PI;
			}

			double raFix = cos(t_rad);
			tx = player.posx/2 + cos(ra) * FLOOR_COEF * 32 / dy / raFix;
			ty = player.posy/2 + sin(ra) * FLOOR_COEF * 32 / dy / raFix;

			int mp = mapFloors[static_cast<int>(ty/32.0) * MAP_WIDTH + static_cast<int>(tx/32.0)] * 32 * 32;
			//double c = Atlas[(static_cast<int>(ty) & 31) * 32 + static_cast<int>(tx) & 31 + mp] * 0.6;
			if(mp!=0){
				pixel=((static_cast<int>(ty) & 31) * 32 + static_cast<int>(tx) & 31)*3+mp*3;
				red=Atlas[pixel]*FLOOR_MOD_R;
				green=Atlas[pixel+1]*FLOOR_MOD_G;
				blue=Atlas[pixel+2]*FLOOR_MOD_B;
				
				sf::Color floorColor(red,green,blue);

				// Add floor quad
				sf::Vector2f topLeftFloor(r * VERT_SIZE, y);
				sf::Vector2f topRightFloor(r * VERT_SIZE + VERT_SIZE, y);
				sf::Vector2f bottomLeftFloor(r * VERT_SIZE, y + 1);
				sf::Vector2f bottomRightFloor(r * VERT_SIZE + VERT_SIZE, y + 1);

				floors.append(sf::Vertex(topLeftFloor, floorColor));
				floors.append(sf::Vertex(topRightFloor, floorColor));
				floors.append(sf::Vertex(bottomRightFloor, floorColor));
				floors.append(sf::Vertex(bottomLeftFloor, floorColor));
			}

			// Ceiling
			mp = mapCeilings[static_cast<int>(ty/32.0) * MAP_WIDTH + static_cast<int>(tx/32.0)] * 32 * 32;
			if(mp!=0){
				//c = Atlas[(static_cast<int>(ty) & 31) * 32 + static_cast<int>(tx) & 31 + mp] * 0.6;
				pixel=((static_cast<int>(ty) & 31) * 32 + static_cast<int>(tx) & 31)*3+mp*3;
				red=Atlas[pixel]*CEILING_MOD_R;
				green=Atlas[pixel+1]*CEILING_MOD_G;
				blue=Atlas[pixel+2]*CEILING_MOD_B;

				sf::Color ceilingColor(red,green,blue);

				sf::Vector2f topLeftCeiling(r * VERT_SIZE, SCREEN_HEIGHT - y);
				sf::Vector2f topRightCeiling(r * VERT_SIZE + VERT_SIZE, SCREEN_HEIGHT - y);
				sf::Vector2f bottomLeftCeiling(r * VERT_SIZE, SCREEN_HEIGHT - y + 1);
				sf::Vector2f bottomRightCeiling(r * VERT_SIZE + VERT_SIZE, SCREEN_HEIGHT - y + 1);

				ceilings.append(sf::Vertex(topLeftCeiling, ceilingColor));
				ceilings.append(sf::Vertex(topRightCeiling, ceilingColor));
				ceilings.append(sf::Vertex(bottomRightCeiling, ceilingColor));
				ceilings.append(sf::Vertex(bottomLeftCeiling, ceilingColor));
				
			}
			
		}

		// Finally, draw all quads in a single batch
		renderer.window.draw(walls);
		renderer.window.draw(floors);
		renderer.window.draw(ceilings);
		
		// Specify the primitive type as sf::Lines
		
		ra+=RAD*RAY_OFFSET;
		if(ra<0){
			ra+=2*PI;
		}
		if(ra>2*PI){
			ra-=2*PI;
		}
	}
	
}

void drawMap2D(Renderer renderer){
	if(draw2d==false){
		return;
	}
	for(int y=0;y<MAP_HEIGHT;y++){
		for(int x=0;x<MAP_WIDTH;x++){
			if(map[x+y*MAP_WIDTH]==1){
				sf::RectangleShape rectangle(sf::Vector2f(CELL_SIZE, CELL_SIZE));

				// change the size to 100x100
				rectangle.setPosition(x*CELL_SIZE,y*CELL_SIZE);
				renderer.window.draw(rectangle);
			}
		}
	}
}

int main() {
    // Create the SFML window
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Biopunk");

    // Initialize Renderer
    Renderer renderer(window);
    renderer.init();

    // Create a Player instance
    Player player(100, 100);
	
	Sprite sp1(2.8,5.8,40.0);

    // Main loop
    while (window.isOpen()==true) {
        sf::Event event;
        while (window.pollEvent(event)==true) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Update player input
        player.update_input();

        // Clear the window
        window.clear(sf::Color::Blue);
		renderer.draw_sky();

        // Draw the player
		if(draw2d==true){
			player.draw(renderer);
			drawMap2D(renderer);
		}
		
		drawRays3D(renderer,player);
		sp1.update(player);
		sp1.drawSpriteToPlayerPersp(renderer,player);

        // Display the window contents
        renderer.window.display();
    }

    return 0;
}
