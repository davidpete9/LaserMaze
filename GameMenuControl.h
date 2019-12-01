#ifndef GAMEMENUCONTROL_H_INCLUDED
#define GAMEMENUCONTROL_H_INCLUDED

ButtonRect **createAllGameButton();
Page clickedOnLevel(int btnId);
ButtonRect ** createInGameButtons();
int getCurrentLevel();
bool isLevelAlreadyDone(int level);
void refreshButtonColors( ButtonRect ** btns);
void drawStarsForLevels(SDL_Renderer * renderer, ButtonRect ** btns);
void drawResultsToFinishPage(SDL_Renderer *renderer);
void drawGameFinishedPage(SDL_Renderer * renderer);

#endif // GAMEMENUCONTROL_H_INCLUDED
