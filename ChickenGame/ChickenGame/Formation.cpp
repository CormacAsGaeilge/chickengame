#include "Formation.hpp"
#include "DataTables.hpp"

namespace
{
	const std::vector<FormationData> Table = initializeFormationData();
}

Formation::Formation()
	: ATeamPositions(21)
	, BTeamPositions(21)
{
	initialiseTeams(FourFourTwo);
}

Formation::~Formation()
{
}

Formation::Formation(Formation::Type formationType)
	: ATeamPositions(21)
	, BTeamPositions(21)
{
	initialiseTeams(formationType);
}

sf::Vector2f Formation::getTeamMemberPosition(bool isATeam, int teamMember) const
{
	if (isATeam) {
		return ATeamPositions.at(teamMember-1) + sf::Vector2f(0.f,3920.f);
	}
	else {
		return BTeamPositions.at(teamMember-1);
	}
}

void Formation::initialiseTeams(Formation::Type formationType)
{
	ATeamPositions.at(0) = Table[formationType].aPlayer1;
	ATeamPositions.at(1) = Table[formationType].aPlayer2;
	ATeamPositions.at(2) = Table[formationType].aPlayer3;
	ATeamPositions.at(3) = Table[formationType].aPlayer4;
	ATeamPositions.at(4) = Table[formationType].aPlayer5;
	ATeamPositions.at(5) = Table[formationType].aPlayer6;
	ATeamPositions.at(6) = Table[formationType].aPlayer7;
	ATeamPositions.at(7) = Table[formationType].aPlayer8;
	ATeamPositions.at(8) = Table[formationType].aPlayer9;
	ATeamPositions.at(9) = Table[formationType].aPlayer10;
	ATeamPositions.at(10) = Table[formationType].aPlayer11;

	BTeamPositions.at(0) = Table[formationType].bPlayer1;
	BTeamPositions.at(1) = Table[formationType].bPlayer2;
	BTeamPositions.at(2) = Table[formationType].bPlayer3;
	BTeamPositions.at(3) = Table[formationType].bPlayer4;
	BTeamPositions.at(4) = Table[formationType].bPlayer5;
	BTeamPositions.at(5) = Table[formationType].bPlayer6;
	BTeamPositions.at(6) = Table[formationType].bPlayer7;
	BTeamPositions.at(7) = Table[formationType].bPlayer8;
	BTeamPositions.at(8) = Table[formationType].bPlayer9;
	BTeamPositions.at(9) = Table[formationType].bPlayer10;
	BTeamPositions.at(10) = Table[formationType].bPlayer11;
}
