#ifndef CELL_H_
#define CELL_H_

class Robot;

enum class Terrain { Floor, Wall, Swamp };

class Cell {
private:
    Terrain terrain = Terrain::Floor;
    bool visible = false; 
    Robot* robot = nullptr;

public:
    Cell(Terrain p_terrain);

    Terrain GetTerrain() const;
    bool IsWalkable() const;
    bool IsVisible() const;
    bool HasRobot() const;

    Robot* GetRobot() const;

    void SetTerrain(Terrain p_terrain);
    void SetVisible(bool p_visible);
    void SetRobot(Robot* other);
    void RemoveRobot();
};

#endif
