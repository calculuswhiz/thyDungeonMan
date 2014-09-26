#include <iostream>
#include <ncurses.h>
#include <cstring>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <SFML/Audio.hpp>

#define DELAYTIME 0x7ffffff

using namespace std;

typedef enum LOCATION
  {
    MAIN_ROOM,
    NORTH,
    SOUTH,
    DENNIS,
    UNDEF
  } t_loc;

int prompt(char * buf);
void toUpper(char * buf, int size);
void process(char * buf);
void look(string args0);
void die();
void get(string args0, string args1);
void go(string args0, string args1);
void talk(string args0);
void give(string args0, string args1);
void help(void);
void delay(void);

int score = 0;
int yeScroll = 0;
t_loc location = UNDEF;
int flask = 0;
int rope = 0;
int dead = 0;
int trinket = 0;

int main()
{
  // Init:
  char promptBuf[255];
  int ch = 0;
  int cmdsize=0;
  int xpos =0, ypos = 0;

  initscr();

  if(has_colors() == TRUE){
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
  }

  curs_set(0);

  // Pre-game:    
  cbreak();
  
  sf::Music music;
  int erc;
  if(erc = !music.openFromFile("../thydungeonman_theme.ogg"))
    printw("Failed to load music.\n");
  else
    music.play();

  keypad(stdscr, TRUE);
  xpos = COLS/2-7;
  ypos = LINES/2-10;
  mvprintw(ypos,xpos,"Thy Dungeonman");
  xpos -= 3;
  ypos += 2;
  mvprintw(ypos++, xpos, "/\\      /\\      /\\");
  mvprintw(ypos++, xpos, "||/----\\||      ||");
  mvprintw(ypos++, xpos++, "\\_------_/      ||");
  mvprintw(ypos++, xpos, "/ o  o \\       ||");
  mvprintw(ypos++, xpos, "/  ||  \\    o__||__o");
  mvprintw(ypos++, xpos, "/ ---- \\     \\----/");
  mvprintw(ypos++, xpos, "/\\/\\/\\/\\       ||");
  mvprintw(ypos++, xpos, "               oo");
  ypos += 2;
  mvprintw(ypos, xpos-4, "Click to enter yon Dungeon");
  refresh();
  
  noecho();
  MEVENT waitclick;
  mousemask(ALL_MOUSE_EVENTS, NULL);
  while(1){
    ch = getch();
    if(ch == KEY_MOUSE){
      if( getmouse(&waitclick) == OK ){
	if( waitclick.bstate & BUTTON1_CLICKED ){
	  break;
	} 
      }
    }
    else if (ch == '\n')
      break;
  }

  // GameStart:
 gamestart:
  if(!erc)
    music.stop();
  echo();
  clear();
  score = 0;
  yeScroll = 0;
  location = MAIN_ROOM;
  flask = 0;
  rope = 0;
  dead = 0;
  trinket = 0;

  delay();
  mvprintw(0,0,"THY DUNGEONMAN\n\nYOU ARE THY DUNGEONMAN!\n\n");
  delay();
  printw("Ye find yeself in yon dungeon. Ye see a SCROLL. Behind ye scroll is a FLASK. Obvious exits are NORTH, SOUTH, and DENNIS.\n");
  delay();
  
  while(dead == 0){
    delay();
    cmdsize = prompt(promptBuf);
    process(promptBuf);
    memset(promptBuf, '\0', cmdsize);

  }

  noecho();
  
  getyx(stdscr,ypos,xpos);
  ypos-=2;

  while( (ch!='n' && ch != 'N' && ch != 'Y' && ch != 'y') || ch == 'a'){
    ch = getch();
    if( ch == KEY_DOWN ){
      score-=100;
      mvprintw(ypos, 0, "Your score was: %d.", score);
      refresh();
    }
  }

  if( ch == 'y' || ch == 'Y')
    goto gamestart;

  if(has_colors() == TRUE){
    attroff(COLOR_PAIR(1));
  }

  refresh();
  endwin();
  return 0;
}

int prompt(char * buf)
{
  // LINES: = maxY, COLS = maxX
  mvprintw(LINES/3, 0, "What wouldst thou deau?\n");
  printw("(\"quit\" or \"exit\" also valid.)\n> ");
  getstr(buf);
  toUpper(buf, strlen(buf));
  move(0,0);
  clear();
  return strlen(buf);
}

void toUpper(char * buf, int size)
{
  int i=0;
  for( ; i<size; i++)
    {
      if( 'a'<=buf[i] and buf[i]<='z' )
	buf[i]-=32;
    }
}

void process(char * buf)
{
  int i=0;
  string cmd = "";
  string args[2] = "";

  // Get command:
  while( buf[i] != ' ' && buf[i] != '\0' )
    {
      cmd += buf[i];
      i++;
    }
  
  // Get args:
  int j=0;
  for( ; j<2; j++){
      while (buf[i] == ' ') {
	  i++;
      }

      while( (buf[i] != ' ' || j==1) && buf[i] != '\0' ){
	args[j]+=buf[i];
	i++;
      }
  }

  if( !cmd.compare(string("HELP")) || !cmd.compare(string("HELPETH")) ){
    help();
  }
  else if (!cmd.compare("DIE")){
    score -= 100;
    mvprintw(0,0,"That wasn't very smart.\nYour score was: %d.\n\nPlay again? [Y/N]", score);
    dead = 1;
  }
  else if (!cmd.compare("SMELL") || !cmd.compare("SNIFF")){
    mvprintw(0,0, "You smell a Wumpus.\n");
  }
  else if (!cmd.compare("LOOK")) {
    if( args[0].empty() ){
      help();
    }
    else
      look(args[0]);
  }
  else if (!cmd.compare("GO")){
    go(args[0], args[1]);
  }
  else if (!cmd.compare("NORTH") || !cmd.compare("SOUTH") || !cmd.compare("WEST") || !cmd.compare("EAST") || !cmd.compare("DENNIS"))
    go(cmd, args[0]);
  else if (!cmd.compare("NOT") && !args[0].compare("DENNIS"))
    go(cmd + " " + args[0], args[1]);
  else if (!cmd.compare("GET") || !cmd.compare("TAKE"))
    get(args[0], args[1]);
  else if (!cmd.compare("GIVE"))
    give(args[0], args[1]);
  else if (!cmd.compare("DRINK")){
    clear();
    mvprintw(0,0,"Thou aren'est thirsty, last time thou checked.\n");
  }
  else if (!cmd.compare("QUIT") || !cmd.compare("EXIT")){
    endwin();
    exit(-1);  // Hahahaha!
  }
  else{
    mvprintw(0,0,"That does not computeth. Type HELP is thou needs of it.\n");
  }
  refresh();
}

void give(string args0, string args1){
  if(!args0.compare("TRINKET") && (args1.empty() || !args1.compare("TO DENNIS")) && trinket > 0 && location == DENNIS){
    clear();
    mvprintw(0,0,"A novel idea! You givst the TRINKET to Dennis and he happily agrees to tell you what parapets are. With this new knowledge, ye escapes from yon dungeon in order to search for new dungeons and to remain...\n\nTHY DUNGEONMAN!!\nYou hath won!\nCongraturation!!\nYour score was: %d\n\n", score);
    delay();
    printw("Play again? [Y/N]");
    dead = 1;
  }
  else{
    clear();
    if(args1.empty())
      args1 = '\b';
    mvprintw(0,0,"Thou don'tst have a %s %s to give. Go back to your tiny life.\n", args0.c_str(), args1.c_str());
  }
}

void get(string args0, string args1){
  int yeFlag = 0;
  if(!args0.compare("YE") || !args0.compare("YON")){
    yeFlag = 1;
    args0.assign(args1);
  }
  
  if(!args0.compare("FLASK") && location == MAIN_ROOM){
    switch(flask){
    case 0:
    case 1:
      clear();
      mvprintw(0,0,"Ye cannot get the FLASK. It is firmly bolted to a wall which is bolted to the rest of the dungeon which is probably bolted to a castle. Never you mind.\n");
      score++;
      flask++;
      break;
    default:
      clear();
      score -= 1000;
      mvprintw(0,0,"Okay, okay. You unbolt yon FLASK and hold it aloft. A great shaking begins. The dungeon ceiling collapses down on you, crushing you in twain. Apparently, this was a load-bearing FLASK.\nYour score was: %d\n\nPlay again? [Y/N]", score);
      dead = 1;
      break;
    }
  }
  else if( !yeFlag && !args0.compare("DAGGER")){
    score += 25;
    clear();
    mvprintw(0,0,"Yeah, okay.\n");
  }
  else if( !args0.compare("SCROLL") && location == MAIN_ROOM ){
    switch(yeScroll){
    case 0:
      score +=2;
      clear();
      mvprintw(0,0,"Ye takes the scroll and reads of it. It doth say:\n\nBEWARE, READER OF THE SCROLL,\nDANGER AWAITS TO THE-\n\nThe SCROLL disappears in thy hands with ye olde ZAP!\n");
      yeScroll++;
      break;
    default:
      yeScroll++;
      score--;
      clear();
      mvprintw(0,0,"Ye doth suffer from memory loss. YE SCROLL is no more. Honestly.\n");
      break;
    }
  }
  else if( !args0.compare("ROPE") && location == NORTH ){
    clear();
    score--;
    mvprintw(0,0,"You attempt to take ye ROPE but alas it is enchanted! It glows a mustard red and smells like a public privy. The ROPE wraps round your neck and hangs you from parapets. With your last breath, you wonder what parapets are.\nYour score was: %d\n\nPlay again? [Y/N]", score);
    dead = 1;
  }
  else if ( !args0.compare("TRINKET") && location == SOUTH ){
    if( trinket == 0 ){
      clear();
      score +=2;
      mvprintw(0,0,"Ye getsts yon TRINKET and discover it to be a bauble. You rejoice at your good fortune. You shove the TRINKET in your pouchel. It kinda hurts.");
      trinket++;
    }
    else if(trinket >= 1){
      clear();
      score--;
      trinket++;
      mvprintw(0,0,"Sigh. The trinket is in thou pouchel. Recallest thou?");
    }
  }
  else{
    clear();
    mvprintw(0,0,"Thou cannotst get that. Quit making stuffeth up!\n");
  }

}

void go(string args0, string args1){
  if(!args0.compare("NORTH")){
    if(location == MAIN_ROOM){
      location = NORTH;
      clear();
      help();
    }
    else if (location == SOUTH){
      location = MAIN_ROOM;
      clear();
      mvprintw(0,0,"You go NORTH\n");
      help();
    }
    else
      mvprintw(0,0,"Thou cannotst go there. Who do you think thou art? A magistrate?!\n");
  }
  else if(!args0.compare("SOUTH")){
    if(location == NORTH){
      location = MAIN_ROOM;
      clear();
      mvprintw(0,0,"You go SOUTH back through yon corridor.\n");
      help();
    }
    else if(location==MAIN_ROOM){
      location = SOUTH;
      clear();
      mvprintw(0,0,"You head south to an enbankment. Or maybe a chasm. You can't decide which. Anyway, ye spies a TRINKET. Obvious exits are NORTH.\n");
      help();
    }
    else
      mvprintw(0,0,"Thou cannotst go there. Who do you think thou art? A magistrate?!\n");
  }
  else if(!args0.compare("DENNIS") && location == MAIN_ROOM){
    location = DENNIS;
    clear();
    help();
  }
  else if(!args0.compare("NOT DENNIS") && location == DENNIS){
    location = MAIN_ROOM;
    clear();
    mvprintw(0,0,"You go NOT DENNIS.\n");
    help();
  }
  else{
    clear();
    mvprintw(0,0,"Thou cannotst go there. Who do you think thou art? A magistrate?!\n");
  }
}

void look(string args0){
  clear();
  if( !args0.compare("SCROLL") && location == MAIN_ROOM ){
    if( yeScroll == 0 )
      mvprintw(0,0,"Parchment, definitely parchment. I'd recognize it anywhere.\n");
    else if ( yeScroll == 1)
      mvprintw(0,0, "Ye seeth nothing wheretofore it went ZAP.\n");
  }
  else if ( !args0.compare("FLASK") && location == MAIN_ROOM ){
    mvprintw(0,0,"Looks like you could quaff some serious mead out of that thing.\n");
  }
  else if ( !args0.compare("PARAPETS") && location == NORTH ){
    mvprintw(0,0,"Well, they're parapets. This much we know for sure.\n");
  }
  else if ( !args0.compare("ROPE") && location == NORTH ){
    mvprintw(0,0,"It looks okay. You've seen better.\n");
  }
  else if ( !args0.compare("TRINKET") && location == SOUTH) {
    if(trinket == 0)
      mvprintw(0,0,"Quit looking! Just get it already.\n");
    else if (trinket == 1)
      mvprintw(0,0,"Just a bulge in thou pouchel at thist point.\n");
  }
  else if ( !args0.compare("DENNIS") && location == DENNIS ){
    mvprintw(0,0,"That jimberjam really makes the outfit.\n");
  }
  else
    mvprintw(0,0,"It looketh pretty awesome.\n");
}

void help(void)
{
  switch(location){
      case MAIN_ROOM:
	if (yeScroll == 0)
	  printw("Ye find yeself in yon dungeon. Ye see a SCROLL. Behind ye scroll is a FLASK. Obvious exits are NORTH, SOUTH and DENNIS.\n");
	else if( yeScroll >= 1 )
	  printw("Ye find yeself in yon dungeon. Back yonder there is a FLASK. Obvious exits are NORTH, SOUTH and DENNIS.\n");
	if( yeScroll >= 2 )
	  printw("There is definitely no YE SCROLL, so drop it.\n");
	break;
      case NORTH:
	printw("You go NORTH through yon corridor. You arrive at parapets. Ye see a ROPE. Obvious exits are SOUTH.\n");
	break;
      case SOUTH:
	if( trinket == 0 )
	  mvprintw(0,0,"You head south to an enbankment. Or maybe a chasm. You can't decide which. Anyway, ye spies a TRINKET. Obvious exits are NORTH.\n");
	else if (trinket == 1)
	  mvprintw(0,0,"Ye stand high above a canyon-like depression. Obvious exits are NORTH.\n");
	else if (trinket >= 2)
	  mvprintw(0,0,"Thou hangeth out at an overlook. Obvious exits are NORTH. I shouldn't have to tell ye there is no TRINKET.\n");
	break;
      case DENNIS:
	printw("Ye arrive at Dennis. He wears a sporty frock coat and a long jimberjam. He paces about nervously. Obvious exits are NOT DENNIS.\n");
	break;
      default:
	mvprintw(0,0,"That's not good.\n");
	break;
  }
}

void delay(void)
{
  int i=0;
  refresh();
  for( i=0; i<DELAYTIME; i++){
    asm("NOP");
  }
}
