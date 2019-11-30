#ifndef GAMEMENUCONTROL_H_INCLUDED
#define GAMEMENUCONTROL_H_INCLUDED

ButtonRect **createAllGameButton();
Page clickedOnLevel(int btnId);
ButtonRect ** createInGameButtons();
int getCurrentLevel();
bool isLevelAlreadyDone(int level);
void markLevelButtonIfPlayable(int level, ButtonRect * btn);
void refreshButtonColors(ButtonRect ** btns);

#endif // GAMEMENUCONTROL_H_INCLUDED
