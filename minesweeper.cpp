/*
 * Project Title:
 * - Minesweeper
 * Description:
 * Recreation of the minesweeper game
 *
 * Developers:
 * - Dylan [REDACTED] - [REDACTED]@mail.uc.edu
 * - Michael [REDACTED] - [REDACTED]@mail.uc.edu
 *
 * Help Received:
 * - some stack overflow posts I have linked below
 *
 * Special Instructions:
 * - I tested this with both MSVC 2017 and MinGW on windows
 *   Note: currently has cool features that are windows only.
 *	 compiler will detect OS and enable these features for you
 *
 * - Tested with g++ on ubuntu
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include <time.h>

#ifdef _WIN32
#define windows
#elif defined(__APPLE__)
#define bash
#elif defined(__linux__)
#define bash
#endif

#ifdef windows
#include <windows.h>
#include <conio.h>
#elif defined(bash)
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#endif

constexpr const char* RED = "\u001b[31;1m";
constexpr const char* GREEN = "\u001b[32;1m";
constexpr const char* RESET = "\u001b[0m";
constexpr const char* WHITE_BOLD = "\u001b[1m";
constexpr const char* YELLOW_HILIGHT = "\u001b[33;1m";

namespace Logic { // Logic forward declerations
	// INCOMPLETE CLASS HEADERS
	class Game; class GameOptions; class GameLoop;  class Board; class Space; class Point;

	///////////
	// ENUMS //
	///////////

	enum class SpaceType {
		REVEALED, EMPTY, MINE, EXPLODED_MINE, FLAGGED_MINE, FALSE_FLAG, INVALID
	};

	enum class ActionResult {
		OK, INVALID, NO_FLAGS, EXPLODED, ALREADY_REVEALED, NOT_HIDDEN
	};

	enum class MinePlacerType {
		RANDOM
	};

	enum class GameState {
		PLAYING, WIN, LOSS
	};

	// STATELESS FUNCTIONS
	int64_t currentTimeSeconds();
}

namespace Render { // Render forward declerations
	// INCOMPLETE CLASS HEADERS
	class OptionsSelector;

	///////////
	// ENUMS //
	///////////
	enum class RenderType {
		CONSOLE
	};

	enum class SelectionMode {
		REVEAL, FLAG, NONE
	};

	// STATELESS FUNCTIONS
	char getCharacter();
}

namespace Logic { // Logic class declarations
	////////////////////////
	// CLASS DECLARATIONS //
	////////////////////////

	/* Class Name: GameOptions
	 *
	 * Class Description:
	 * This class instantiates the options for the Game, which the user chooses the value for these,
	 *
	 */
	class GameOptions {
	public:
		int width, height, mines;
		MinePlacerType minePlacerType;
	};

	/* Class Name: Point
	 *
	 * Class Description:
	 * This Class defines the Point object. This is used for referencing the location with co-ordinates
	 * without having to call each co-ordinate individually
	 */
	class Point {
	public:
		int x, y;
		Point(int x, int y);
		Point add(int x, int y);
		bool operator == (Point p);
	};

	/* Class Name: Space
	 *
	 * Class Description:
	 * This Class defines the Space object. This is used for referencing each
	 * space on the board, getting its location and type.
	 */
	class Space {
	private:
		Board& board;
		Point location;
		SpaceType type;
	public:
		Space(Board& board, Point location);
		int getAdjacentMines();
		bool click();
		bool flag();
		bool adjacentTo(Space space);
		bool operator == (Space space);
		SpaceType getType();
		Point getLocation();
		void setType(SpaceType type);
	};

	/* Class Name: Board
	 *
	 * Class Description:
	 * This Class defines the Board object. This is to create each board
	 *
	 */
	class Board {
	private:
		Game& game;
		GameOptions options;
		std::vector<std::vector<Space>> cols;
		Space invalid;
	public:
		Board(Game& game, GameOptions options);
		Space& getSpaceAt(Point location);
		Space& getSpaceAt(int x, int y);
		int getUsedFlags();
		GameOptions getOptions();
		std::vector<std::vector<Space>>& getCols();
		Game& getGame();
	};

	/* Class Name: Game
	 *
	 * Class Description:
	 * This Class defines the Game object. This is to create each game
	 */
	class Game {
	private:
		GameLoop& loop;
		GameOptions options;
		Board board;
		bool firstMove;
		int64_t startTime;
	public:
		Game(GameLoop& loop, GameOptions options);
		int getRemainingFlags();
		ActionResult clickSpace(Point location);
		ActionResult flagSpace(Point location);
		Board& getBoard();
		GameOptions getOptions();
		GameState getState();
		int64_t getStartTime();
		GameLoop& getLoop();
		bool isFirstMove();
	};

	/* Class Name: MinePlacer
	 *
	 * Class Description:
	 * This Class places all the mines on the board
	 *
	 */
	class MinePlacer {
	protected:
		Board& board;
	public:
		MinePlacer(Board& board) : board(board) {}
		virtual ~MinePlacer();
		virtual void placeMines(Logic::Point clear) = 0;
	};

	/* Class Name: RandomMinePlacer
	 *
	 * Class Description:
	 * This Class is a derived class of Mineplacer and places all the mines on the board
	 *
	 */
	class RandomMinePlacer : MinePlacer {
	public:
		RandomMinePlacer(Board& board);
		void placeMines(Logic::Point clear);
	};

	/* Class Name: GameLoop
	 *
	 * Class Description:
	 * This Class creates a loop that will loop until the game is finished
	 *
	 */
	class GameLoop {
	private:
		Render::RenderType renderType;
		Render::OptionsSelector* optionsSelector;
		std::vector<int64_t> winTimes, lossTimes;
		int wins, losses, flagged, exploded, revealed;
	public:
		GameLoop(Render::RenderType renderType);
		~GameLoop();
		void start();
		void addRevealed();
		void addExploded();
		void addFlagged();
		void addWin(int64_t time);
		void addLoss(int64_t time);
		std::vector<int64_t>& getWinTimes();
		std::vector<int64_t>& getLossTimes();
		int getWins();
		int getLosses();
		int getFlagged();
		int getExploded();
		int getRevealed();
	};
}

namespace Render { // Render class declarations
	////////////////////////
	// CLASS DECLARATIONS //
	////////////////////////

	/* Class Name: Renderer
	 *
	 * Class Description:
	 * This virtual Class renders is the base class to keep all inputs and outputs and keeps it organized
	 *
	 *
	 */
	class Renderer {
	public:
		virtual ~Renderer();
		virtual void initializeRender() = 0;
		virtual void drawBoard(Logic::Game& game) = 0;
		virtual bool promptForInput(Logic::Game& game) = 0;
		virtual void clear() = 0;
		virtual bool promptPlayAgain() = 0;
		virtual bool hasAdvancedRendering() = 0;
		virtual void displayStats(Logic::GameLoop& loop) = 0;
		virtual void playGame(Logic::GameLoop& loop, Logic::Game& game) = 0;
		virtual void displayMenu(Logic::GameLoop& loop) = 0;
	};

	/* Class Name: ConsoleRenderer
	 *
	 * Class Description:
	 * This derived Class is derived from the Renderer Class and renders all inputs and outputs and keeps it organized
	 *
	 */
	class ConsoleRenderer : public Renderer {
	private:
		Logic::Point selection;
		SelectionMode selectionMode;
		SelectionMode lastAction;
	public:
		ConsoleRenderer();
		void initializeRender();
		void drawBoard(Logic::Game& game);
		bool promptForInput(Logic::Game& game);
		void clear();
		void displayStats(Logic::GameLoop& loop);
		bool promptPlayAgain();
		bool hasAdvancedRendering();
		void playGame(Logic::GameLoop& loop, Logic::Game& game);
		void displayMenu(Logic::GameLoop& loop);
	};

	/* Class Name: OptionsSelector
	 *
	 * Class Description:
	 * This virtual Class is used as a base to allow the user to select the options
	 *
	 */
	class OptionsSelector {
	public:
		virtual Logic::GameOptions promptForOptions() = 0;
		virtual ~OptionsSelector();
	};

	/* Class Name: ConsoleOptionsSelector
	 *
	 * Class Description:
	 * This derived Class is derived from the OptionsSelector Classs and is used to allow the user to select the options
	 *
	 */
	class ConsoleOptionsSelector : public OptionsSelector {
	public:
		Logic::GameOptions promptForOptions();
	};
}

namespace Logic { // Logic class implementations

	/////////////////////////////////////////////
	// CLASS DEFINITION: Point
	/////////////////////////////////////////////

	Point::Point(int x, int y) : x(x), y(y) {}

	inline Point Point::add(int x, int y) {
		return Point(this->x + x, this->y + y);
	}

	/* Function Name: operator == ()
	 *
	 * Function Description:
	 * This function changes the operator == to get location and check against the location
	 *
	 * Parameters:
	 * The p parameter is a point type which indecates the position
	 *
	 * return value:
	 * the updated operator
	 */
	inline bool Point::operator == (Point p) {
		return p.x == x && p.y == y;
	}

	/////////////////////////////////////////////
	// CLASS DEFINITION: Space
	/////////////////////////////////////////////

	Space::Space(Board& board, Point location) : board(board), location(location) {
		type = SpaceType::EMPTY;
	}

	/* Function Name: getAdjacentMines()
	 *
	 * Function Description:
	 * This function returns the number of adjacent mines
	 *
	 * Parameters:
	 * none
	 *
	 * return value:
	 * The number of adjacent mines
	 */
	int Space::getAdjacentMines() {
		int count = 0;
		for (int xOffset = -1; xOffset <= 1; xOffset++) {
			for (int yOffset = -1; yOffset <= 1; yOffset++) {
				if (xOffset == 0 && yOffset == 0) {
					continue;
				}
				Point access = location.add(xOffset, yOffset);
				if (access.x < 0 || access.y < 0 || access.x >= board.getOptions().width || access.y >= board.getOptions().height) {
					continue;
				}
				Space& space = board.getSpaceAt(access);
				if (space.getType() == SpaceType::MINE || space.getType() == SpaceType::FLAGGED_MINE) {
					count++;
				}
			}
		}
		return count;
	}

	/* Function Name: click()
	 *
	 * Function Description:
	 * This function updates the state after the the point is chosen
	 *
	 * Parameters:
	 * none
	 *
	 * return value:
	 * true if someting was updated, false if not
	 */
	inline bool Space::click() {
		if (type == SpaceType::MINE) {
			board.getGame().getLoop().addExploded();
			for (std::vector<Space>& row : board.getCols()) {
				for (Space& space : row) {
					if (space.getType() == SpaceType::MINE) {
						space.setType(SpaceType::EXPLODED_MINE);
					}
					if (space.getType() == SpaceType::EMPTY || space.getType() == SpaceType::FALSE_FLAG) {
						space.setType(SpaceType::REVEALED);
					}
				}
			}
			return true;
		}
		if (type == SpaceType::EMPTY) {
			type = SpaceType::REVEALED;
			board.getGame().getLoop().addRevealed();
			if (getAdjacentMines() == 0) {
				for (std::vector<Space>& row : board.getCols()) {
					for (Space& space : row) {
						if (space.adjacentTo(*this)) {
							space.click();
						}
					}
				}
			}
			return true;
		}
		return false;
	}

	/* Function Name: flag()
	 *
	 * Function Description:
	 * This function updates the state for the flags after the the point is chosen
	 *
	 * Parameters:
	 * none
	 *
	 * return value:
	 * true if someting was updated, false if not
	 */
	inline bool Space::flag() {
		if (type == SpaceType::MINE) {
			board.getGame().getLoop().addFlagged();
			type = SpaceType::FLAGGED_MINE;
			return true;
		}
		if (type == SpaceType::EMPTY) {
			board.getGame().getLoop().addFlagged();
			type = SpaceType::FALSE_FLAG;
			return true;
		}
		if (type == SpaceType::FALSE_FLAG) {
			type = SpaceType::EMPTY;
			return true;
		}
		if (type == SpaceType::FLAGGED_MINE) {
			type = SpaceType::MINE;
			return true;
		}
		return false;
	}

	/* Function Name: adjacentTo()
	 *
	 * Function Description:
	 * This function checks if the space is adjacent to the spaces location
	 *
	 * Parameters:
	 * The space parameter is the position of the space object on the board
	 *
	 * return value:
	 * if the location is adjacent to the current space
	 */
	inline bool Space::adjacentTo(Space space) {
		return !(*this == space) && (abs(location.x - space.getLocation().x) <= 1 && abs(location.y - space.getLocation().y) <= 1);
	}

	/* Function Name: operator == ()
	 *
	 * Function Description:
	 * This function changes the operator == to get location and check against the location
	 *
	 * Parameters:
	 * The space parameter is the position of the space object on the board
	 *
	 * return value:
	 * the updated operator
	 */
	inline bool Space::operator == (Space space) {
		return space.getLocation() == location;
	}

	/* Function Name: getType()
	 *
	 * Function Description:
	 * This function gets the type of space
	 *
	 * Parameters:
	 * none
	 *
	 * return value:
	 * type
	 */
	inline SpaceType Space::getType() {
		return type;
	}

	/* Function Name: getLocation()
	 *
	 * Function Description:
	 * This function gets the location
	 *
	 * Parameters:
	 * none
	 *
	 * return value:
	 * type
	 */
	inline Point Space::getLocation() {
		return location;
	}

	/* Function Name: setType()
	 *
	 * Function Description:
	 * This function sets the type of space
	 *
	 * Parameters:
	 * The type parameter is the type of space object on the board
	 *
	 * return value:
	 * none
	 */
	inline void Space::setType(SpaceType type) {
		this->type = type;
	}

	/////////////////////////////////////////////
	// CLASS DEFINITION: Board
	/////////////////////////////////////////////

	/* Function Name: Board()
	 *
	 * Function Description:
	 * This function creates the board
	 *
	 * Parameters:
	 * The options parameter is the options of the board
	 * The game parameter is the reference to the game class
	 *
	 * return value:
	 * none
	 */
	Board::Board(Game& game, GameOptions options) : game(game), options(options), invalid(*this, Point(-1, -1)) {
		for (int x = 0; x < options.width; x++) {
			std::vector<Space> row;
			for (int y = 0; y < options.height; y++) {
				Point location(x, y);
				Space space(*this, location);
				row.push_back(space);
			}
			cols.push_back(row);
		}
	}

	/* Function Name: getSpaceAt()
	 *
	 * Function Description:
	 * This gets the Space type at a location
	 *
	 * Parameters:
	 * The location parameter is a point type
	 *
	 * return value:
	 * space
	 */
	Space& Board::getSpaceAt(Point location) {
		if (location.x >= 0 && location.y >= 0 && location.x < options.width && location.y < options.height) {
			Space& space = cols.at(static_cast<unsigned int>(location.x)).at(static_cast<unsigned int>(location.y));
			if (space.getLocation() == location) {
				return space;
			}
		}
		std::cout << "WARNING: ACCESSED INVALID SPACE AT (" << location.x << ", " << location.y << ")!" << std::endl;
		return invalid;
	}

	/* Function Name: getUsedFlags()
	 *
	 * Function Description:
	 * This gets the number of flags used
	 *
	 * Parameters:
	 * none
	 *
	 * return value:
	 * space
	 */
	int Board::getUsedFlags() {
		int count = 0;
		for (std::vector<Space>& row : cols) {
			for (Space& space : row) {
				if (space.getType() == SpaceType::FALSE_FLAG || space.getType() == SpaceType::FLAGGED_MINE) {
					count++;
				}
			}
		}
		return count;
	}

	inline Space& Board::getSpaceAt(int x, int y) {
		Point location(x, y);
		return getSpaceAt(location);
	}

	inline GameOptions Board::getOptions() {
		return options;
	}

	inline std::vector<std::vector<Space>>& Board::getCols() {
		return cols;
	}

	inline Game& Board::getGame() {
		return game;
	}

	/////////////////////////////////////////////
	// CLASS DEFINITION: Game
	/////////////////////////////////////////////

	Game::Game(GameLoop& loop, GameOptions options) : loop(loop), options(options), board(*this, options) {
		this->firstMove = true;
		this->startTime = currentTimeSeconds();
	}

	/* Function Name: clickSpace()
	 *
	 * Function Description:
	 * This determines what happens when you select a space
	 *
	 * Parameters:
	 * The location parameter is a point type
	 *
	 * return value:
	 * ActionResult
	 */
	ActionResult Game::clickSpace(Point location) {
		if (firstMove) {
			this->firstMove = false;
			if (options.minePlacerType == MinePlacerType::RANDOM) {
				RandomMinePlacer placer(board);
				placer.placeMines(location);
			}
		}
		Space& target = board.getSpaceAt(location);
		if (!target.click()) {
			return ActionResult::ALREADY_REVEALED;
		}
		if (target.getType() == SpaceType::EXPLODED_MINE) {
			return ActionResult::EXPLODED;
		}
		return ActionResult::OK;
	}

	/* Function Name: flagSpace()
	 *
	 * Function Description:
	 * This checks if there is a flag placed at a certain location
	 *
	 * Parameters:
	 * The location parameter is a point type
	 *
	 * return value:
	 * ActionResult
	 */
	ActionResult Game::flagSpace(Point location) {
		Space& target = board.getSpaceAt(location);
		if (!target.flag()) {
			return ActionResult::NOT_HIDDEN;
		}
		return ActionResult::OK;
	}

	/* Function Name: getState()
	 *
	 * Function Description:
	 * This checks the state of the game. If there are exploded mines, it returns a loss immediately.
	 * If all flags are in the correct spot and all have been selected, the player wins
	 *
	 * Parameters:
	 * none
	 *
	 * return value:
	 * ActionResult
	 */
	GameState Game::getState() {
		int flags = 0;
		for (std::vector<Space>& row : board.getCols()) {
			for (Space& space : row) {
				if (space.getType() == SpaceType::EXPLODED_MINE) {
					return GameState::LOSS; // any exploded mine is an instant loss
				}
				if (space.getType() == SpaceType::FLAGGED_MINE) {
					flags++;
				}
			}
		}
		if (flags == options.mines) { // all mines must be flagged for win state
			return GameState::WIN;
		}
		return GameState::PLAYING;
	}

	inline int Game::getRemainingFlags() {
		return options.mines - board.getUsedFlags();
	}

	inline Board& Game::getBoard() {
		return board;
	}

	inline GameOptions Game::getOptions() {
		return options;
	}

	inline int64_t Game::getStartTime() {
		return startTime;
	}

	inline GameLoop& Game::getLoop() {
		return loop;
	}

	inline bool Game::isFirstMove() {
		return firstMove;
	}

	// MinePlacer: Trivia virtual destructor
	MinePlacer::~MinePlacer() {}

	/////////////////////////////////////////////
	// CLASS DEFINITION: Random Mine Placer
	/////////////////////////////////////////////

	RandomMinePlacer::RandomMinePlacer(Board& board) : MinePlacer(board) {}

	/* Function Name: placeMines()
	 *
	 * Function Description:
	 * Places mines on the board
	 *
	 * Parameters:
	 * The clear parameter is a point type
	 *
	 * return value:
	 * none
	 */
	void RandomMinePlacer::placeMines(Logic::Point clear) {
		GameOptions options = board.getOptions();
		int minesToPlace = options.mines;
		if (minesToPlace > options.width * options.height - 1) {
			minesToPlace = options.width * options.height - 1;
		}
		while (minesToPlace > 0) {
			std::vector<Space*> notMined;
			for (std::vector<Space>& row : board.getCols()) {
				for (Space& space : row) {
					if (space.getType() != SpaceType::MINE) {
						notMined.push_back(&space);
					}
				}
			}
			Space& space = *notMined.at(static_cast<unsigned int>(rand()) % notMined.size());
			if (space.getType() != SpaceType::MINE && !(space.getLocation() == clear)) {
				if (notMined.size() > 9 && space.adjacentTo(board.getSpaceAt(clear))) { // avoid putting mines around starting location at all cost
					continue;
				}
				space.setType(SpaceType::MINE);
				minesToPlace--;
			}
		}
	}

	/////////////////////////////////////////////
	// CLASS DEFINITION: Game Loop
	/////////////////////////////////////////////

	GameLoop::GameLoop(Render::RenderType renderType) : renderType(renderType) {
		if (renderType == Render::RenderType::CONSOLE) {
			this->optionsSelector = new Render::ConsoleOptionsSelector();
		}
	}

	GameLoop::~GameLoop() {
		delete optionsSelector;
	}

	/* Function Name: start()
	 *
	 * Function Description:
	 * This function starts the game
	 *
	 * return value:
	 * none
	 */
	void GameLoop::start() {
		Render::Renderer* renderer = nullptr;
		if (renderType == Render::RenderType::CONSOLE) {
			renderer = new Render::ConsoleRenderer();
		}

		if (renderer->hasAdvancedRendering()) {
			renderer->displayMenu(*this);
		}
		else {
			while (true) {
				GameOptions options = optionsSelector->promptForOptions();
				Game game(*this, options);
				renderer->playGame(*this, game);
				if (!renderer->promptPlayAgain()) {
					break;
				}
			}
			renderer->displayStats(*this);
		}

		delete renderer;
		std::cout << "Press (ENTER) to exit..." << std::endl;
		Render::getCharacter();
	}

	inline void GameLoop::addExploded() {
		exploded++;
	}

	inline void GameLoop::addFlagged() {
		flagged++;
	}

	inline void GameLoop::addRevealed() {
		revealed++;
	}

	inline void GameLoop::addWin(int64_t time) {
		wins++;
		winTimes.push_back(time);
	}

	inline void GameLoop::addLoss(int64_t time) {
		losses++;
		lossTimes.push_back(time);
	}

	inline std::vector<int64_t>& GameLoop::getWinTimes() {
		return winTimes;
	}

	inline std::vector<int64_t>& GameLoop::getLossTimes() {
		return lossTimes;
	}

	inline int GameLoop::getWins() {
		return wins;
	}

	inline int GameLoop::getLosses() {
		return losses;
	}

	inline int  GameLoop::getFlagged() {
		return flagged;
	}

	inline int GameLoop::getExploded() {
		return exploded;
	}

	inline int GameLoop::getRevealed() {
		return revealed;
	}
}

namespace Render {
	// Renderer: Trivial virtual destructor
	Renderer::~Renderer() {}

	// Render class implementations (Console)

	// Things used for drawing to the screen
#define CONSOLE_H_BAR u8"══"
#define CONSOLE_V_BAR u8"║"
#define CONSOLE_TL_CORNER u8"╔"
#define CONSOLE_TR_CORNER u8"╗"
#define CONSOLE_BL_CORNER u8"╚"
#define CONSOLE_BR_CORNER u8"╝"
#define CONSOLE_VBOX u8"██"
#define CONSOLE_VSELECT u8"▒"
#define CONSOLE_FLAG u8"√"
#define CONSOLE_MINE u8"Φ"

	ConsoleRenderer::ConsoleRenderer() : selection(Logic::Point(0, 0)) {
		this->selectionMode = SelectionMode::NONE;
		this->lastAction = SelectionMode::REVEAL;
	}

	void ConsoleRenderer::initializeRender() {

	}

	/* Function Name: drawBoard()
	 *
	 * Function Description:
	 * This outputs the board
	 *
	 * Parameters:
	 * The game parameter references the Game class
	 *
	 * return value:
	 * none
	 */
	void ConsoleRenderer::drawBoard(Logic::Game& game) {
		Logic::Board& board = game.getBoard();
		Logic::GameOptions options = game.getOptions();
		Logic::GameState state = game.getState();
		std::vector<std::string> lines;
		std::string start, end, numbers, spikes;
		for (int y = 0; y < options.height; y++) {
			spikes = "    ";
			numbers = "    ";
			start = "   ";
			start += CONSOLE_TL_CORNER;
			end = "   ";
			end += +CONSOLE_BL_CORNER;
			std::string row;
			if (y % 2 == 0) {
				std::string strNum = std::to_string(y + 1);
				if (strNum.size() == 1) {
					strNum = "0" + strNum;
				}
				row += strNum + "-" + CONSOLE_V_BAR;
			}
			else {
				row += "   ";
				row += CONSOLE_V_BAR;
			}
			for (int x = 0; x < options.width; x++) {
				bool color = selectionMode != SelectionMode::NONE && Logic::Point(x, y) == selection;
				if (x % 2 == 0) {
					std::string strNum = std::to_string(x + 1);
					if (strNum.size() == 1) {
						strNum = "0" + strNum;
					}
					numbers += strNum + "  ";
					spikes += "|   ";
				}
				start += CONSOLE_H_BAR;
				end += CONSOLE_H_BAR;
				Logic::Space& space = board.getSpaceAt(x, y);
				std::string dispCount = std::to_string(space.getAdjacentMines());
				if (dispCount == "0" || state == Logic::GameState::LOSS) {
					dispCount = " ";
				}
				if (color) {
					if (selectionMode == SelectionMode::REVEAL) {
						row += RED;
					}
					else {
						row += GREEN;
					}
				}
				switch (space.getType()) {
				case Logic::SpaceType::EXPLODED_MINE:
					row += CONSOLE_MINE;
					break;
				case Logic::SpaceType::REVEALED:
					if (dispCount == " " && color) {
						dispCount = CONSOLE_VSELECT;
					}
					row += dispCount;
					break;
				case Logic::SpaceType::MINE:
				case Logic::SpaceType::EMPTY:
					row += CONSOLE_VBOX;
					break;
				case Logic::SpaceType::FALSE_FLAG:
				case Logic::SpaceType::FLAGGED_MINE:
					row += CONSOLE_FLAG;
					break;
				default:
					row += " ";
				}
				if (color) {
					row += RESET;
				}
				if (space.getType() != Logic::SpaceType::EMPTY && space.getType() != Logic::SpaceType::MINE) {
					row += " ";
				}
			}
			row += CONSOLE_V_BAR;
			lines.push_back(row);
		}
		start += CONSOLE_TR_CORNER;
		end += CONSOLE_BR_CORNER;
		lines.insert(lines.begin(), start);
		lines.insert(lines.begin(), spikes);
		lines.insert(lines.begin(), numbers);
		lines.push_back(end);

		for (std::string line : lines) {
			std::cout << line << std::endl;
		}

		if (game.getState() == Logic::GameState::PLAYING) {
			int64_t timeDifference = Logic::currentTimeSeconds() - game.getStartTime();
			int flags = game.getRemainingFlags();
			std::string time = "Time: " + std::to_string(timeDifference) + " seconds";
			std::string output = "   Remaining Flags: " + std::to_string(flags);
			if (game.getOptions().width >= 20) {
				struct Helper {
					static unsigned int stringLen(std::string str) {
						const char* s = str.c_str();
						int len = 0;
						while (*s) len += (*s++ & 0xc0) != 0x80;
						return len;
					}
				};
				unsigned int maxLineLen = Helper::stringLen(lines.at(lines.size() - 1));
				unsigned int spaces = maxLineLen - Helper::stringLen(output) - Helper::stringLen(time);
				for (unsigned int i = 0; i < spaces; i++) {
					output += " ";
				}
			}
			else {
				output += "\n   ";
			}
			output += time;
			std::cout << output << std::endl;
		}

		std::cout << std::endl;
	}

	/* Function Name: promptForInput()
	 *
	 * Function Description:
	 * This function prompts for all inputs and handles them. Returns true if it is a proper input
	 *
	 * Parameters:
	 * The game parameter is passed by reference
	 *
	 * return value:
	 * bool
	 */
	bool ConsoleRenderer::promptForInput(Logic::Game& game) {
		enum class Command {
			REVEAL, FLAG, QUIT, INVALID
		};
		Logic::GameOptions options = game.getOptions();
		Command selected = Command::INVALID;
		char hackThatInput = 0;
		while (selected == Command::INVALID) {
			std::cout << "What action would you like to perform? Choose from (r)eveal, (f)lag, or (q)uit: ";
			char command = getCharacter();
			if (command == 0) {
				std::cout << "Please type something..." << std::endl;
			}
			switch (command) {
			case 'r':
				this->lastAction = SelectionMode::REVEAL;
				selected = Command::REVEAL;
				break;
			case 'f':
				if (game.isFirstMove()) {
					std::cout << "You cannot set a flag on your first move!" << std::endl;
					continue;
				}
				this->lastAction = SelectionMode::FLAG;
				selected = Command::FLAG;
				break;
			case 'q':
				selected = Command::QUIT;
				break;
#if defined(windows) || defined(bash)
			case 'w':
			case 'a':
			case 's':
			case 'd':
			case 72:
			case 80:
			case 75:
			case 77:
				hackThatInput = command;
				switch (lastAction) {
				case SelectionMode::FLAG:
					selected = Command::FLAG;
					break;
				case SelectionMode::REVEAL:
					selected = Command::REVEAL;
					break;
				default:
					break;
				}
				break;
#endif
			default:
				std::cout << "That was not a valid command." << std::endl;
				break;
			}
		}
		if (selected == Command::QUIT) {
			clear();
			drawBoard(game);
			return true;
		}
#if !defined(windows) && !defined(bash)
		std::cout << "Now you will input the X and Y positions of the square to target." << std::endl;
		int x = 0, y = 0;
		while (x < 1 || x > options.width) {
			std::cout << "X: ";
			std::cin >> x;
			std::cin.clear();
			std::cin.ignore(100000, '\n');
			if (x < 1 || x > options.width) {
				std::cout << "You must enter a value in the range of the width (1-" << options.width << ")." << std::endl;
			}
		}
		while (y < 1 || y > options.height) {
			std::cout << "Y: ";
			std::cin >> y;
			std::cin.clear();
			std::cin.ignore(100000, '\n');
			if (y < 1 || y > options.height) {
				std::cout << "You must enter a value in the range of the height (1-" << options.height << ")." << std::endl;
			}
		}
		Logic::Point location(x - 1, y - 1);
#else
		this->selectionMode = (selected == Command::REVEAL ? SelectionMode::REVEAL : SelectionMode::FLAG);
		while (true) {
			if (!hackThatInput) {
				clear();
				drawBoard(game);
				std::cout << "Use keys (w, a, s, d) to change selection and press (ENTER) to select square." << std::endl;
				std::string exitStr = "r";
				if (selected == Command::FLAG) {
					exitStr = "f";
				}
				std::cout << "Press (" << exitStr << ") to cancel selection." << std::endl;
			}
			int xOffset = 0, yOffset = 0;
			char input = hackThatInput ? hackThatInput : getCharacter();
			switch (input) {
			case 10: // lunux enter (linefeed)
			case 13: // windows enter (return)
				goto finishSelection;
				break;
			case 'r':
			case 'f':
				selected = Command::INVALID;
				goto finishSelection;
				break;
			case 72:
			case 'w':
				yOffset--;
				break;
			case 80:
			case 's':
				yOffset++;
				break;
			case 75:
			case 'a':
				xOffset--;
				break;
			case 77:
			case 'd':
				xOffset++;
				break;
			}
			hackThatInput = 0;
			Logic::Point newLoc = selection.add(xOffset, yOffset);
			if (newLoc.x >= 0 && newLoc.y >= 0 && newLoc.x < options.width && newLoc.y < options.height) {
				selection = newLoc;
			}
		}
	finishSelection:
		this->selectionMode = SelectionMode::NONE;
		Logic::Point location = selection;
#endif
		Logic::ActionResult result = Logic::ActionResult::INVALID;
		if (selected == Command::REVEAL) {
			result = game.clickSpace(location);
		}
		else if (selected == Command::FLAG) {
			result = game.flagSpace(location);
		}
		clear();
		drawBoard(game);
		if (result == Logic::ActionResult::ALREADY_REVEALED) {
			std::cout << "That square could not be revealed since it isn't hidden!" << std::endl;
		}
		else if (result == Logic::ActionResult::NO_FLAGS) {
			std::cout << "That square could not be flagged as you have no remaining flags!" << std::endl;
		}
		else if (result == Logic::ActionResult::NOT_HIDDEN) {
			std::cout << "That square could not be flagged as it is already revealed!" << std::endl;
		}
		return false;
	}

	constexpr int minWH = 4;
	constexpr int maxWH = 50;
	constexpr int minMines = 2;

	// OptionsSelector: Trivial virtual destructor
	OptionsSelector::~OptionsSelector() {}

	/* Function Name: promptForOptions()
	 *
	 * Function Description:
	 * This function prompts for all options and handles them. Returns true if it is a proper input
	 *
	 *
	 * return value:
	 * options
	 */
	Logic::GameOptions ConsoleOptionsSelector::promptForOptions() {
		Logic::GameOptions options;
		options.minePlacerType = Logic::MinePlacerType::RANDOM;
		std::cout << std::endl << " == New Game Options == " << std::endl;
		do {
			std::cout << "Please enter a board width (" << minWH << "-" << maxWH << "): ";
			std::cin >> options.width;
			std::cin.clear();
			std::cin.ignore(100000, '\n');
			if (options.width < minWH || options.width > maxWH) {
				std::cout << "Please choose a width between " << minWH << " and " << maxWH << "." << std::endl;
			}
		} while (options.width < minWH || options.width > maxWH);
		do {
			std::cout << "Please enter a board height (" << minWH << "-" << maxWH << "): ";
			std::cin >> options.height;
			std::cin.clear();
			std::cin.ignore(100000, '\n');
			if (options.height < minWH || options.height > maxWH) {
				std::cout << "Please choose a height between " << minWH << " and " << maxWH << "." << std::endl;
			}
		} while (options.height < minWH || options.height > maxWH);
		int maxMines = options.width * options.height - 10; // buffer starting area
		do {
			std::cout << "Please enter a mine count (" << minWH << "-" << maxMines << ", recommended: " << static_cast<int>(maxMines * 0.12) << "): ";
			std::cin >> options.mines;
			std::cin.clear();
			std::cin.ignore(100000, '\n');
			if (options.mines < minMines || options.mines > maxMines) {
				std::cout << "Please choose a mine count between " << minMines << " and " << maxMines << "." << std::endl;
			}
		} while (options.mines < minMines || options.mines > maxMines);
		return options;
	}

	/* Function Name: displayMenu()
	 *
	 * Function Description:
	 * This function prompts for all options and handles them. Returns true if it is a proper input
	 *
	 * Parameters:
	 * The loop parameter is passed by reference
	 *
	 * return value:
	 * options
	 */
	void ConsoleRenderer::displayMenu(Logic::GameLoop& loop) {
		constexpr int maxSelected = 4;
		constexpr const char* menuOptions[maxSelected + 1] = { "Start Game", "Options   ", "Statistics", "Rules     ", "Exit      " };
		Logic::GameOptions options;
		options.width = 20;
		options.height = 20;
		options.mines = 100;
		options.minePlacerType = Logic::MinePlacerType::RANDOM;
		while (true) {
			int selected = 0;
			while (true) {
				clear();
				std::cout << std::endl << WHITE_BOLD << " Minesweeper - Main Menu" << RESET << std::endl << std::endl;
				for (int index = 0; index < maxSelected + 1; index++) {
					std::string result;
					if (index == selected) {
						result += YELLOW_HILIGHT;
						result += "=== ";
					}
					else {
						result += "--- ";
					}
					result += menuOptions[index];
					if (index == selected) {
						result += " === ";
					}
					else {
						result += " --- ";
					}
					result += RESET;
					std::cout << result << std::endl;
				}
				std::cout << std::endl << "Navigate the menu using the (w, s) keys, and make a selection by pressing (ENTER)." << std::endl << std::endl;
				std::cout << "(C) 2019" << std::endl;
				std::cout << "Game by [redacted for github] and [redacted for github]" << std::endl;
				switch (getCharacter()) {
				case 72:
				case 'w':
					if (selected == 0) {
						break;
					}
					selected--;
					break;
				case 80:
				case 's':
					if (selected == maxSelected) {
						break;
					}
					selected++;
					break;
				case 10: // lunux enter (linefeed)
				case 13: // windows enter (return)
					goto finishSelection;
					break;
				}
			}
		finishSelection:;
			switch (selected) {
			case 0:
			{ // Scope for some c++ transfer of control bypasses initialization
				Logic::Game game(loop, options);
				selection = Logic::Point(0, 0);
				playGame(loop, game);
				std::cout << std::endl << "Press (ENTER) to return to the main menu." << std::endl;
				while (true) {
					switch (getCharacter()) {
					case 10: case 13:
						goto returnToMainMenu;
						break;
					}
				}
			}
			break;
			case 1:
			{
				while (true) {
					constexpr int maxOptionsSelected = 3;
					int optionsSelected = 0;
					while (true) {
						clear();
						std::cout << std::endl << WHITE_BOLD << " Options" << RESET << std::endl << std::endl;
						for (int index = 0; index < maxOptionsSelected + 1; index++) {
							std::string result;
							if (index == optionsSelected) {
								result += YELLOW_HILIGHT;
								result += "= ";
							}
							else {
								result += "- ";
							}
							switch (index) {
							case 0:
								result += "Board Width: " + std::to_string(options.width);
								break;
							case 1:
								result += "Board Height: " + std::to_string(options.height);
								break;
							case 2:
								result += "Mine Count: " + std::to_string(options.mines);
								break;
							case 3:
								result += "Back to Main Menu";
								break;
							}
							result += RESET;
							std::cout << result << std::endl;
						}
						std::cout << std::endl << "Navigate the menu using the (w, s) keys, and make a selection by pressing (ENTER)." << std::endl;
						std::cout << "Press (q) to return to the main menu." << std::endl;
						switch (getCharacter()) {
						case 72:
						case 'w':
							if (optionsSelected == 0) {
								break;
							}
							optionsSelected--;
							break;
						case 80:
						case 's':
							if (optionsSelected == maxOptionsSelected) {
								break;
							}
							optionsSelected++;
							break;
						case 10: // lunux enter (linefeed)
						case 13: // windows enter (return)
							goto finishOptionsSelection;
							break;
						case 'q':
							goto returnToMainMenu;
							break;
						}
					}
				finishOptionsSelection:;
					const char* valueType = "Undefined";
					int currentValue = 0;
					int minValue = 0;
					int maxValue = 0;
					int editedValue = -1;
					switch (optionsSelected) {
					case 0:
						valueType = "Board Width";
						minValue = 3;
						maxValue = 50;
						currentValue = options.width;
						break;
					case 1:
						valueType = "Board Height";
						minValue = 3;
						maxValue = 50;
						currentValue = options.height;
						break;
					case 2:
						valueType = "Mine Count";
						minValue = 2;
						maxValue = (options.width * options.height) - 8;
						if (maxValue < minValue) {
							maxValue = minValue;
						}
						currentValue = options.mines;
						break;
					case 3:
						goto returnToMainMenu;
						break;
					}
					clear();
					std::cout << "You are editing the option \"" << valueType << "\". Current value: " << currentValue << std::endl;
					do {
						std::cout << "Please enter a value between " << minValue << " and " << maxValue << ": ";
						std::cin >> editedValue;
						std::cin.clear();
						std::cin.ignore(100000, '\n');
						if (editedValue < minValue || editedValue > maxValue) {
							std::cout << "That value was out of range." << std::endl;
						}
					} while (editedValue < minValue || editedValue > maxValue);
					switch (optionsSelected) {
					case 0:
						options.width = editedValue;
						break;
					case 1:
						options.height = editedValue;
						break;
					case 2:
						options.mines = editedValue;
					}
				}
			}
			break;
			case 2:
			{
				clear();
				displayStats(loop);
				std::cout << std::endl << "Press (ENTER) to return to the main menu." << std::endl;
				while (true) {
					switch (getCharacter()) {
					case 10: case 13:
						goto returnToMainMenu;
						break;
					}
				}

			}
			break;
			case 3:
			{
				// Rules from http://www.freeminesweeper.org/help/minehelpinstructions.html
				clear();
				std::cout << "A squares \"neighbours\" are the squares adjacent above, below, left, right, and all 4 diagonals." << std::endl;
				std::cout << "Squares on the sides of the board or in a corner have fewer neighbors. "
					<< "The board does not wrap around the edges." << std::endl;
				std::cout << "If you open a square with 0 neighboring bombs, all its neighbors will automatically open."
					<< "This can cause a large area to automatically open." << std::endl;
				std::cout << "The first square you open is never a bomb." << std::endl;
				std::cout << "If you mark a bomb incorrectly, you will have to correct the mistake before you can win. "
					<< "Incorrect bomb marking doesn't kill you, but it can lead to mistakes which do." << std::endl;
				std::cout << std::endl << "Press (ENTER) to return to the main menu." << std::endl;
				while (true) {
					switch (getCharacter()) {
					case 10: case 13:
						goto returnToMainMenu;
						break;
					}
				}
			}
			case 4:
				clear();
				return;
			}
		returnToMainMenu:;
			if (options.mines >= options.width * options.height) {
				options.mines = options.width * options.height - 1;
			}
		}
	}

	/* Function Name: promptPlayAgain()
	 *
	 * Function Description:
	 * This function prompts if user wants to play again
	 *
	 * Parameters:
	 * none
	 *
	 * return value:
	 * bool
	 */
	bool ConsoleRenderer::promptPlayAgain() {
		std::cout << std::endl << "Would you like to play again? (y/n): ";
		int state = -1;
		while (state == -1) {
			char input = getCharacter();
			switch (input) {
			case 'y':
				state = 1;
				break;
			case 'n':
				state = 0;
				break;
			default:
				std::cout << "Please type (y) or (n)." << std::endl;
				break;
			}
		}
		clear();
		return state;
	}

	/* Function Name: displayStats()
	 *
	 * Function Description:
	 * This function displays the stats of past games
	 *
	 * Parameters:
	 * The loop parameter is passed by reference
	 *
	 * return value:
	 * none
	 */
	void ConsoleRenderer::displayStats(Logic::GameLoop& loop) {
		int gamesPlayed = loop.getWins() + loop.getLosses();
		std::cout << " == Player Stats == " << std::endl;
		std::cout << "Total spaces revealed: " << loop.getRevealed() << std::endl;
		std::cout << "Total mines exploded: " << loop.getExploded() << std::endl;
		std::cout << "Total spaces flagged: " << loop.getFlagged() << std::endl;
		std::cout << std::endl;
		std::cout << "Total games: " << gamesPlayed << std::endl;
		std::cout << "Total wins: " << loop.getWins() << std::endl;
		std::cout << "Total losses: " << loop.getLosses() << std::endl;
		std::cout << std::endl;
		if (gamesPlayed > 0) {
			std::cout << std::setprecision(2) << std::fixed;
			std::cout << "Win percentage: " << (loop.getWins() / static_cast<double>(gamesPlayed)) * 100 << "%" << std::endl;
			if (loop.getWinTimes().size() > 0) {
				int64_t totalTime = 0;
				for (int64_t time : loop.getWinTimes()) {
					totalTime += time;
				}
				std::cout << "Average time to win: " << (totalTime / static_cast<double>(loop.getWinTimes().size())) << " seconds" << std::endl;
			}
			std::cout << "Loss percentage: " << (loop.getLosses() / static_cast<double>(gamesPlayed)) * 100 << "%" << std::endl;
			if (loop.getLossTimes().size() > 0) {
				int64_t totalTime = 0;
				for (int64_t time : loop.getLossTimes()) {
					totalTime += time;
				}
				std::cout << "Average time to loss: " << (totalTime / static_cast<double>(loop.getLossTimes().size())) << " seconds" << std::endl;
			}
			std::cout << std::endl;
		}
	}

	/* Function Name: playGame()
	 *
	 * Function Description:
	 * This funstion plays the game
	 *
	 * Parameters:
	 * The loop parameter is passed by reference
	 * The game parameter is passed by reference
	 *
	 * return value:
	 * none
	 */
	void ConsoleRenderer::playGame(Logic::GameLoop& loop, Logic::Game& game) {
		clear();
		drawBoard(game);
		while (game.getState() == Logic::GameState::PLAYING) {
			if (promptForInput(game)) {
				break;
			}
		}
		if (game.getState() == Logic::GameState::WIN) {
			loop.addWin(Logic::currentTimeSeconds() - game.getStartTime());
			std::cout << R"( __   __           __        ___       _ )" << std::endl;
			std::cout << R"( \ \ / /__  _   _  \ \      / (_)_ __ | |)" << std::endl;
			std::cout << R"(  \ V / _ \| | | |  \ \ /\ / /| | '_ \| |)" << std::endl;
			std::cout << R"(   | | (_) | |_| |   \ V  V / | | | | |_|)" << std::endl;
			std::cout << R"(   |_|\___/ \__,_|    \_/\_/  |_|_| |_(_))" << std::endl;
		}
		else if (game.getState() == Logic::GameState::LOSS) {
			loop.addLoss(Logic::currentTimeSeconds() - game.getStartTime());
			std::cout << R"( __   __            _              _   )" << std::endl;
			std::cout << R"( \ \ / /__  _   _  | |    ___  ___| |_ )" << std::endl;
			std::cout << R"(  \ V / _ \| | | | | |   / _ \/ __| __|)" << std::endl;
			std::cout << R"(   | | (_) | |_| | | |__| (_) \__ \ |_ )" << std::endl;
			std::cout << R"(   |_|\___/ \__,_| |_____\___/|___/\__|)" << std::endl;
		}
	}

	inline void ConsoleRenderer::clear() {
#ifdef windows
		system("cls");
#elif defined(bash)
		system("clear");
#endif
	}

	inline char getCharacter() {
		char result;
#if defined(windows)
		result = -32;
		while (result == -32) {
			result = static_cast<char>(_getch());
		}
#elif defined(bash)
		// https://stackoverflow.com/a/9547954/4634429
		struct termios old_tio, new_tio;
		tcgetattr(STDIN_FILENO, &old_tio);
		new_tio = old_tio;
		new_tio.c_lflag &= (~ICANON & ~ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
		result = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
#else
		std::string buffer;
		std::cin >> buffer;
		if (buffer.size() == 0) {
			result = 0;
		}
		else {
			result = buffer.at(0);
		}
#endif
		return result;
	}

	inline bool ConsoleRenderer::hasAdvancedRendering() {
#if defined(windows) || defined(bash)
		return true;
#else
		return false;
#endif
	}
}

// STATELESS FUNCTIONS
namespace Logic {
	inline int64_t currentTimeSeconds() {
		// https://stackoverflow.com/a/19555298/4634429
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
}

/* Function Name: main()
 *
 * Function Description:
 * maximizes te console window to the size of the screen and starts the loop
 *
 * return value:
 * 0
 */
int main() {
#ifdef windows
	SetConsoleOutputCP(65001);
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
#endif

	srand(static_cast<unsigned int>(time(nullptr)));
	Logic::GameLoop gameLoop(Render::RenderType::CONSOLE);
	gameLoop.start();

	return 0;
}
