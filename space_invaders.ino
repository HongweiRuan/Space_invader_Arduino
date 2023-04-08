#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 16;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// define Delay
const int CANNON_BALL_DELAY = 25;
const int INVADER_DELAY = 5000;

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void game_over();
class Color {
public:
  int red;
  int green;
  int blue;
  Color() {
    red = 0;
    green = 0;
    blue = 0;
  }
  Color(int r, int g, int b) {
    red = r;
    green = g;
    blue = b;
  }
  uint16_t to_333() const {
    return matrix.Color333(red, green, blue);
  }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

class Invader {
public:
  // Constructors
  Invader() {
    x = 0;
    y = 0;
    strength = 0;
    time = millis();
  }
  // sets values for private date members x and y
  Invader(int x_arg, int y_arg) {
    x = x_arg;
    y = y_arg;
    time = millis();
  }
  // sets values for private data members
  Invader(int x_arg, int y_arg, int strength_arg) {
    x = x_arg;
    y = y_arg;
    strength = strength_arg;
    time = millis();
  }
  // sets values for private data members
  void initialize(int x_arg, int y_arg, int strength_arg, bool reached_arg) {
    x = x_arg;
    y = y_arg;
    strength = strength_arg;
    reached = false;
    time = millis();
  }

  // getters
  int get_x() const {
    return x;
  }
  int get_y() const {
    return y;
  }
  int get_strength() const {
    return strength;
  }

  // Moves the down the screen by one row
  // Modifies: y
  void move() {
    if (strength > 0 && (millis() - time) > INVADER_DELAY) {
      erase();
      y++;
      if (y >= 12) {
        reached = true;
      } else {
        draw();
      }
      time = millis();
    }
  }

  // draws the if its strength is greater than 0
  // calls: draw_with_rgb
  void draw() {
    if (strength > 0) {
      if (strength == 1) {
        draw_with_rgb(RED, BLUE);
      } else if (strength == 2) {
        draw_with_rgb(ORANGE, BLUE);
      } else if (strength == 3) {
        draw_with_rgb(YELLOW, BLUE);
      } else if (strength == 4) {
        draw_with_rgb(GREEN, BLUE);
      } else if (strength == 5) {
        draw_with_rgb(BLUE, BLUE);
      } else if (strength == 6) {
        draw_with_rgb(PURPLE, BLUE);
      } else if (strength == 7) {
        draw_with_rgb(WHITE, BLUE);
      }
    }
  }

  bool reachedBottom() {
    return reached;
  }

  bool isHit(int x_arg, int y_arg) {
    bool flag = false;
    if(strength == 0){
      return false;
    }
    if ((x_arg == x-1 && y_arg == y + 3) ||(x_arg == x && y_arg == y + 2) || (x_arg == x + 1 && y_arg == y + 2) || (x_arg == x + 2 && y_arg == y + 3)) {
      flag = true;
    }
    return flag;
  }

  bool collide(int x_arg, int y_arg) {
    bool flag = false;
    if ((x_arg == x - 1 && y_arg == y + 3) || (x_arg == x + 2 && y_arg == y + 3) || (x_arg == x && y_arg == y + 2) || (x_arg == x + 1 && y_arg == y + 2)) {
      flag = true;
    }
    return flag;
  }

  // draws black where the used to be
  // calls: draw_with_rgb
  void erase() {
    draw_with_rgb(BLACK, BLACK);
  }

  // is hit by a Cannonball.
  // Modifies: strength
  // calls: draw, erase
  void hit() {
    strength = strength - 1;
    erase();
    draw();
  }

private:
  int x;
  int y;
  int strength;
  bool reached = false;
  unsigned long time;
  // draws the Invader
  void draw_with_rgb(Color body_color, Color eye_color) {
    matrix.drawPixel(x, y, body_color.to_333());
    matrix.drawPixel(x + 1, y, body_color.to_333());
    matrix.drawPixel(x - 1, y + 1, body_color.to_333());
    matrix.drawPixel(x, y + 1, eye_color.to_333());
    matrix.drawPixel(x + 1, y + 1, eye_color.to_333());
    matrix.drawPixel(x + 2, y + 1, body_color.to_333());
    matrix.drawPixel(x - 1, y + 2, body_color.to_333());
    matrix.drawPixel(x, y + 2, body_color.to_333());
    matrix.drawPixel(x + 1, y + 2, body_color.to_333());
    matrix.drawPixel(x + 2, y + 2, body_color.to_333());
    matrix.drawPixel(x - 1, y + 3, body_color.to_333());
    matrix.drawPixel(x + 2, y + 3, body_color.to_333());
  }
};

class Cannonball {
public:
  Cannonball() {
    x = 0;
    y = 0;
    fired = false;
    time = millis();
  }

  // resets private data members to initial values
  void reset() {
    x = 0;
    y = 0;
    fired = false;
    time = millis();
  }

  // getters
  int get_x() const {
    return x;
  }
  int get_y() const {
    return y;
  }
  bool has_been_fired() const {
    return fired;
  }

  // sets private data members
  void fire(int x_arg, int y_arg) {
    x = x_arg;
    y = y_arg;
    fired = true;
  }

  // moves the Cannonball and detects if it goes off the screen
  // Modifies: y, fired
  void move() {
    // y--;
    // if (y < 0) {
    //   reset();
    // }
    if (fired && (millis() - time) > CANNON_BALL_DELAY) {
      erase();
      y--;
      if (y < 0) {
        reset();
      } else {
        draw();
      }
      time = millis();
    }
  }

  // resets private data members to initial values
  void hit() {
    reset();
    erase();
  }

  // draws the Cannonball, if it is fired
  void draw() {
    matrix.drawPixel(x, y, ORANGE.to_333());
    matrix.drawPixel(x, y - 1, ORANGE.to_333());
  }

  // draws black where the Cannonball used to be
  void erase() {
    matrix.drawPixel(x, y, BLACK.to_333());
    matrix.drawPixel(x, y + 1, BLACK.to_333());
  }

private:
  int x;
  int y;
  bool fired;
  unsigned long time;
};

class Player {
public:
  Player() {
    x = 1;
    y = 14;
    lives = 3;
  }

  // getters
  int get_x() const {
    return x;
  }
  int get_y() const {
    return y;
  }
  int get_lives() const {
    return lives;
  }

  // setter
  void set_x(int x_arg) {
    x = x_arg;
  }

  // Modifies: lives
  void die() {
    lives = lives - 1;
  }

  // draws the Player
  // calls: draw_with_rgb
  void draw() {
    draw_with_rgb(AQUA);
  }

  // draws black where the Player used to be
  // calls: draw_with_rgb
  void erase() {
    draw_with_rgb(BLACK);
  }

private:
  int x;
  int y;
  int lives;

  // sets private data members x and y to initial values
  void initialize(int x_arg, int y_arg) {
    x = x_arg;
    y = y_arg;
  }

  // draws the player
  void draw_with_rgb(Color color) {
    matrix.drawPixel(x, y, color.to_333());
    matrix.drawPixel(x - 1, y + 1, color.to_333());
    matrix.drawPixel(x, y + 1, color.to_333());
    matrix.drawPixel(x + 1, y + 1, color.to_333());
  }
};

class Game {

public:
  Game() {
    level = 1;
    time = 0;
  }

  // Sets up a new game of Space Invaders
  // Modifies: global variable matrix
  void setupGame() {
    matrix.fillScreen(BLACK.to_333());
    delay(50);
    display_status();
    if (level == 1) {
      num_enemies = 8;
      enemies[0].initialize(1, 0, 1, false);
      enemies[0].draw();
      enemies[1].initialize(5, 0, 1, false);
      enemies[1].draw();
      enemies[2].initialize(9, 0, 1, false);
      enemies[2].draw();
      enemies[3].initialize(13, 0, 1, false);
      enemies[3].draw();
      enemies[4].initialize(17, 0, 1, false);
      enemies[4].draw();
      enemies[5].initialize(21, 0, 1, false);
      enemies[5].draw();
      enemies[6].initialize(25, 0, 1, false);
      enemies[6].draw();
      enemies[7].initialize(29, 0, 1, false);
      enemies[7].draw();
    } else if (level == 2) {
      num_enemies = 16;
      enemies[0].initialize(1, 0, 1, false);
      enemies[0].draw();
      enemies[1].initialize(5, 0, 2, false);
      enemies[1].draw();
      enemies[2].initialize(9, 0, 1, false);
      enemies[2].draw();
      enemies[3].initialize(13, 0, 2, false);
      enemies[3].draw();
      enemies[4].initialize(17, 0, 1, false);
      enemies[4].draw();
      enemies[5].initialize(21, 0, 2, false);
      enemies[5].draw();
      enemies[6].initialize(25, 0, 1, false);
      enemies[6].draw();
      enemies[7].initialize(29, 0, 2, false);
      enemies[7].draw();
      enemies[8].initialize(1, 5, 1, false);
      enemies[8].draw();
      enemies[9].initialize(5, 5, 1, false);
      enemies[9].draw();
      enemies[10].initialize(9, 5, 2, false);
      enemies[10].draw();
      enemies[11].initialize(13, 5, 1, false);
      enemies[11].draw();
      enemies[12].initialize(17, 5, 1, false);
      enemies[12].draw();
      enemies[13].initialize(21, 5, 2, false);
      enemies[13].draw();
      enemies[14].initialize(25, 5, 1, false);
      enemies[14].draw();
      enemies[15].initialize(29, 5, 2, false);
      enemies[15].draw();
    } else if (level == 3) {
      num_enemies = 16;
      enemies[0].initialize(1, 0, 1, false);
      enemies[0].draw();
      enemies[1].initialize(5, 0, 2, false);
      enemies[1].draw();
      enemies[2].initialize(9, 0, 3, false);
      enemies[2].draw();
      enemies[3].initialize(13, 0, 4, false);
      enemies[3].draw();
      enemies[4].initialize(17, 0, 5, false);
      enemies[4].draw();
      enemies[5].initialize(21, 0, 1, false);
      enemies[5].draw();
      enemies[6].initialize(25, 0, 2, false);
      enemies[6].draw();
      enemies[7].initialize(29, 0, 3, false);
      enemies[7].draw();
      enemies[8].initialize(1, 5, 4, false);
      enemies[8].draw();
      enemies[9].initialize(5, 5, 5, false);
      enemies[9].draw();
      enemies[10].initialize(9, 5, 1, false);
      enemies[10].draw();
      enemies[11].initialize(13, 5, 2, false);
      enemies[11].draw();
      enemies[12].initialize(17, 5, 3, false);
      enemies[12].draw();
      enemies[13].initialize(21, 5, 4, false);
      enemies[13].draw();
      enemies[14].initialize(25, 5, 5, false);
      enemies[14].draw();
      enemies[15].initialize(29, 5, 1, false);
      enemies[15].draw();
    } else if (level == 4) {
      num_enemies = 16;
      enemies[0].initialize(1, 0, 5, false);
      enemies[0].draw();
      enemies[1].initialize(5, 0, 4, false);
      enemies[1].draw();
      enemies[2].initialize(9, 0, 5, false);
      enemies[2].draw();
      enemies[3].initialize(13, 0, 4, false);
      enemies[3].draw();
      enemies[4].initialize(17, 0, 5, false);
      enemies[4].draw();
      enemies[5].initialize(21, 0, 4, false);
      enemies[5].draw();
      enemies[6].initialize(25, 0, 5, false);
      enemies[6].draw();
      enemies[7].initialize(29, 0, 4, false);
      enemies[7].draw();
      enemies[8].initialize(1, 5, 2, false);
      enemies[8].draw();
      enemies[9].initialize(5, 5, 3, false);
      enemies[9].draw();
      enemies[10].initialize(9, 5, 2, false);
      enemies[10].draw();
      enemies[11].initialize(13, 5, 3, false);
      enemies[11].draw();
      enemies[12].initialize(17, 5, 2, false);
      enemies[12].draw();
      enemies[13].initialize(21, 5, 3, false);
      enemies[13].draw();
      enemies[14].initialize(25, 5, 2, false);
      enemies[14].draw();
      enemies[15].initialize(29, 5, 3, false);
      enemies[15].draw();
    } else if (level >= 5) {
      num_enemies = 16;
      enemies[0].initialize(1, 0, random(1, 8), false);
      enemies[0].draw();
      enemies[1].initialize(5, 0, random(1, 8), false);
      enemies[1].draw();
      enemies[2].initialize(9, 0, random(1, 8), false);
      enemies[2].draw();
      enemies[3].initialize(13, 0, random(1, 8), false);
      enemies[3].draw();
      enemies[4].initialize(17, 0, random(1, 8), false);
      enemies[4].draw();
      enemies[5].initialize(21, 0, random(1, 8), false);
      enemies[5].draw();
      enemies[6].initialize(25, 0, random(1, 8), false);
      enemies[6].draw();
      enemies[7].initialize(29, 0, random(1, 8), false);
      enemies[7].draw();
      enemies[8].initialize(1, 5, random(1, 8), false);
      enemies[8].draw();
      enemies[9].initialize(5, 5, random(1, 8), false);
      enemies[9].draw();
      enemies[10].initialize(9, 5, random(1, 8), false);
      enemies[10].draw();
      enemies[11].initialize(13, 5, random(1, 8), false);
      enemies[11].draw();
      enemies[12].initialize(17, 5, random(1, 8), false);
      enemies[12].draw();
      enemies[13].initialize(21, 5, random(1, 8), false);
      enemies[13].draw();
      enemies[14].initialize(25, 5, random(1, 8), false);
      enemies[14].draw();
      enemies[15].initialize(29, 5, random(1, 8), false);
      enemies[15].draw();
    }
    player.draw();
  }

  // Advances the game simulation one step and renders the graphics
  // See spec for details of game
  // Modifies: global variable matrix
  void update(int potentiometer_value, bool button_pressed) {

    if (player.get_lives() < 1) {
      game_over();
      delay(2000);
      exit(0);
    }

    // Update the position of the player based on the value of the potentiometer
    player.erase();
    player.set_x(potentiometer_value / 32);
    player.draw();

    // Update the position of the cannonball, including detecting if a new cannonball is being fired
    if (button_pressed && !ball.has_been_fired()) {
      ball.fire(player.get_x(), 14);
    }
    ball.move();
    ball.erase();

    // Update the position of each invader
    for (int i = 0; i < num_enemies; i++) {
      if (num_enemies == 16) {
        enemies[i].draw();
        if (i >= 8) {
          enemies[i].move();
        } else {
          bool flag = true;
          for (int i = 8; i < 16; i++) {
            if (enemies[i].get_strength() != 0) {
              flag = false;
            }
          }
          if (flag) {
            enemies[i].move();
          }
        }
      }else{
        enemies[i].draw();
        enemies[i].move();
      }


      // Detect if the cannonball is colliding with any of the invaders;
      // if so, both the cannonball and the invader disappear
      if (ball.has_been_fired()) {
        if (enemies[i].isHit(ball.get_x(), ball.get_y())) {
          // redraw the enemy
          enemies[i].hit();
          // erase the ball
          ball.hit();

          // Detect if all the invaders are killed; if so, start the next level
          if (level_cleared()) {
            Serial.println("Level Cleared!");
            level++;
            setupGame();
          }
        }
      }

      // Detect if the player is colliding with any of the invaders; if so, the player loses a life and the level restarts
      if (enemies[i].collide(player.get_x(), player.get_y()) || enemies[i].reachedBottom()) {
        player.die();
        if (player.get_lives() == 0) {
          game_over();
          break;
        } else {
          setupGame();
        }
      }
    }
  }


private:
  int level;
  int num_enemies;
  unsigned long time;
  Player player;
  Cannonball ball;
  Invader enemies[NUM_ENEMIES];

  // Add this new function to display level and lives
  void display_status() {
    matrix.fillScreen(BLACK.to_333());
    matrix.setCursor(0, 0);
    print_level(level);
    delay(1000);
    matrix.fillScreen(BLACK.to_333());
    delay(500);
    matrix.setCursor(0, 0);
    print_lives(player.get_lives());
    delay(1000);
    matrix.fillScreen(BLACK.to_333());
  }


  // Check if Player defeated all Invaders in the current level
  bool level_cleared() {
    if (level == 1) {
      for (int n = 0; n < 8; n++) {
        if (enemies[n].get_strength() != 0) {
          return false;
        }
      }
    } else {
      for (int n = 0; n < 16; n++) {
        if (enemies[n].get_strength() != 0) {
          return false;
        }
      }
    }
    return true;
  }
};


// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(4));
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  game.setupGame();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);
  game.update(potentiometer_value, button_pressed);
}

// displays Level
void print_level(int level) {
  matrix.print("Level: ");
  matrix.println(level);
}

// displays number of lives
void print_lives(int lives) {
  matrix.print("Lives: ");
  matrix.println(lives);
}

// displays "game over"
void game_over() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(0, 0);
  matrix.print("Game over");
  delay(10000);
  matrix.fillScreen(BLACK.to_333());
}