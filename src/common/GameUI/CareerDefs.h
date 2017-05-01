#ifndef COMMON_GAMEUI_CAREERDEFS_H
#define COMMON_GAMEUI_CAREERDEFS_H

#include <vector>

enum CareerStateType
{
	CAREER_NONE = 0,
	CAREER_LOADING,
	CAREER_PLAYING
};

//TODO: define - Solokiller
enum CareerDifficultyType
{
};

//TODO: define - Solokiller
struct MapInfo
{

};

//TODO: define - Solokiller
class ICareerTask;

typedef std::vector<ICareerTask*> ITaskVec;

#endif //COMMON_GAMEUI_CAREERDEFS_H
