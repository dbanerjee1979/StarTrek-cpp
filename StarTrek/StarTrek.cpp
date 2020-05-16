#include <iostream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include "Time.h"
#include "Random.h"
#include "Enterprise.h"
#include "Galaxy.h"
#include "Events.h"
#include "Star.h"
#include "Course.h"

class StarTrek : public Events {
private:
    Random m_rnd;
    Galaxy m_galaxy;
    std::shared_ptr<Enterprise> m_ent;
    Time& m_time;
    std::string m_cmd;
    std::unordered_map<std::string, std::function<void()>> m_commands;
    std::unordered_map<int, std::function<void()>> m_computer_commands;
    bool m_resign;
public:
    StarTrek() :
        m_galaxy(m_rnd, *this),
        m_ent(m_galaxy.enterprise()),
        m_time(m_ent->time()),
        m_resign(false) {
        m_commands["NAV"] = std::bind(&StarTrek::navigate, this);
        m_commands["SRS"] = std::bind(&StarTrek::short_range_scan, this);
        m_commands["LRS"] = std::bind(&StarTrek::long_range_scan, this);
        m_commands["PHA"] = std::bind(&StarTrek::fire_phasers, this);
        m_commands["TOR"] = std::bind(&StarTrek::fire_torpedoes, this);
        m_commands["SHE"] = std::bind(&StarTrek::raise_shields, this);
        m_commands["DAM"] = std::bind(&StarTrek::damage_control_report, this);
        m_commands["COM"] = std::bind(&StarTrek::computer, this);
        m_commands["XXX"] = std::bind(&StarTrek::resign, this);

        m_computer_commands[0] = std::bind(&StarTrek::galactic_record, this);
        m_computer_commands[1] = std::bind(&StarTrek::status_report, this);
        m_computer_commands[2] = std::bind(&StarTrek::torpedo_data, this);
        m_computer_commands[3] = std::bind(&StarTrek::starbase_data, this);
        m_computer_commands[4] = std::bind(&StarTrek::dir_dist_calc, this);
        m_computer_commands[5] = std::bind(&StarTrek::galactic_map, this);
    }

    void run(bool& keep_playing) {
        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl
            << std::endl << std::endl << std::endl << std::endl << std::endl;
        std::cout << "                                    ,------*------," << std::endl;
        std::cout << "                    ,-------------   '---  ------' " << std::endl;
        std::cout << "                     '-------- --'      / /        " << std::endl;
        std::cout << "                         ,---' '-------/ /--,      " << std::endl;
        std::cout << "                          '----------------'       " << std::endl;
        std::cout << std::endl;
        std::cout << "                    THE USS ENTERPRISE --- NCC-1701" << std::endl;
        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl;

        bool plural = m_galaxy.total_bases() > 1;
        std::cout << "YOUR ORDERS ARE AS FOLLOWS:" << std::endl;
        std::cout << "     DESTROY THE " << m_galaxy.total_klingons() << " KLINGON WARSHIPS WHICH HAVE INVADED" << std::endl;
        std::cout << "   THE GALAXY BEFORE THEY CAN ATTACK FEDERATION HEADQUARTERS" << std::endl;
        std::cout << "   ON STARDATE " << m_time.deadline() << ".  THIS GIVES YOU " << m_time.max_days() << " DAYS.  THERE "
            << (plural ? "ARE" : "IS") << std::endl;
        std::cout << "  " << m_galaxy.total_bases() << " STARBASE" << (plural ? "S" : "") << " IN THE GALAXY FOR RESUPPLYING YOUR SHIP" << std::endl;
        std::cout << std::endl;

        m_ent->enter_quadrant();
        short_range_scan();

        bool enough_energy = true;
        try {
            while (!m_resign && (enough_energy = m_ent->has_enough_energy()) && m_galaxy.total_klingons() > 0) {
                try {
                    std::cout << "COMMAND ";
                    read_cmd();
                    auto it = m_commands.find(m_cmd);
                    if (it != m_commands.end()) {
                        it->second();
                    }
                    else {
                        std::cout << std::boolalpha << std::cin.bad() << std::endl;
                        std::cout << "  NAV  (TO SET COURSE)" << std::endl;
                        std::cout << "  SRS  (FOR SHORT RANGE SENSOR SCAN)" << std::endl;
                        std::cout << "  LRS  (FOR LONG RANGE SENSOR SCAN)" << std::endl;
                        std::cout << "  PHA  (TO FIRE PHASERS)" << std::endl;
                        std::cout << "  TOR  (TO FIRE PHOTON TORPEDOES)" << std::endl;
                        std::cout << "  SHE  (TO RAISE OR LOWER SHIELDS)" << std::endl;
                        std::cout << "  DAM  (FOR DAMAGE CONTROL REPORTS)" << std::endl;
                        std::cout << "  COM  (TO CALL ON LIBRARY-COMPUTER)" << std::endl;
                        std::cout << "  XXX  (TO RESIGN YOUR COMMAND)" << std::endl;
                    }
                }
                catch (NoKlingonsException&) {
                    std::cout << "SCIENCE OFFICER SPOCK REPORTS  'SENSORS SHOW NO ENEMY SHIPS" << std::endl;
                    std::cout << "                                IN THIS QUADRANT'" << std::endl;
                }
            }
        }
        catch (EnterpriseDestroyedException&) {
            std::cout << std::endl;
            std::cout << "THE ENTERPRISE HAS BEEN DESTROYED.  THEN FEDERATION " << std::endl;
            std::cout << "WILL BE CONQUERED" << std::endl;
        }
        catch (DeadlineReachedException&) {
            // end game
        }
        catch (NoBasesAndNoTimeException&) {
            std::cout << "THAT DOES IT, CAPTAIN!!  YOU ARE HEREBY RELIEVED OF COMMAND" << std::endl;
            std::cout << "AND SENTENCED TO 99 STARDATES AT HARD LABOR ON CYGNUS 12!!" << std::endl;
        }
        if (!enough_energy) {
            std::cout << std::endl;
            std::cout << "** FATAL ERROR **   YOU'VE JUST STRANDED YOUR SHIP IN " << std::endl;
            std::cout << "SPACE. YOU HAVE INSUFFICIENT MANEUVERING ENERGY," << std::endl;
            std::cout << " AND SHIELD CONTROL IS PRESENTLY INCAPABLE OF CROSS" << std::endl;
            std::cout << "-CIRCUITING TO ENGINE ROOM!!" << std::endl;
        }
        if (!m_resign) {
            std::cout << "IT IS STARDATE " << std::fixed << std::setprecision(1) << *m_time << std::endl;
        }
        std::cout << "THERE WERE " << m_galaxy.total_klingons() << " KLINGON BATTLE CRUISERS LEFT AT" << std::endl;
        std::cout << "THE END OF YOUR MISSION." << std::endl;
        std::cout << std::endl << std::endl;
        if (m_galaxy.total_bases() == 0) {
            keep_playing = false;
        }
        else {
            std::cout << "THE FEDERATION IS IN NEED OF A NEW STARSHIP COMMANDER" << std::endl;
            std::cout << "FOR A SIMILAR MISSION -- IF THERE IS A VOLUNTEER," << std::endl;
            std::cout << "LET HIM STEP FORWARD AND ENTER 'AYE' ";
            read_cmd();
            keep_playing = m_cmd == "AYE";
        }
    }

    void read_cmd() {
        std::getline(std::cin, m_cmd);
        std::transform(m_cmd.begin(), m_cmd.end(), m_cmd.begin(), ::toupper);
    }

    void navigate() {
        try {
            std::string input;
            m_ent->navigate(
                [&](int min, int max) {
                    std::cout << "COURSE (" << min << "-" << max << ") ";
                    std::getline(std::cin, input);
                    return std::stod(input);
                },
                [&](double min, double max) {
                    std::cout << "WARP FACTOR (" << min << "-" << max << ") ";
                    std::getline(std::cin, input);
                    return std::stod(input);
                }
                );
            short_range_scan();
        }
        catch (InvalidCourseException&) {
            std::cout << "   LT. SULU REPORTS, 'INCORRECT COURSE DATA, SIR!'" << std::endl;
        }
        catch (InvalidWarpFactorException & w) {
            if (w.damaged() && w.warp() > w.max()) {
                std::cout << "WARP ENGINES ARE DAMAGED.  MAXIUM SPEED = WARP " << w.max() << std::endl;
            }
            else {
                std::cout << "   CHIEF ENGINEER SCOTT REPORTS 'THE ENGINES WON'T TAKE" << std::endl;
                std::cout << "WARP " << w.warp() << "!'" << std::endl;
            }
        }
        catch (InsufficientNavigationEnergyException & e) {
            std::cout << "ENGINEERING REPORTS   'INSUFFICIENT ENERGY AVAILABLE";
            std::cout << "                       FOR MANEUVERING AT WARP" << e.warp() << "!'";
            int s = e.shields_available();
            if (s > 0) {
                std::cout << "DEFLECTOR CONTROL ROOM ACKNOWLEDGES" << s << "UNITS OF ENERGY" << std::endl;
                std::cout << "                         PRESENTLY DEPLOYED TO SHIELDS." << std::endl;
            }
        }
        catch (std::invalid_argument&) {
            // do nothing
        }
    }

    void short_range_scan() {
        try {
            int r = 0;
            SectorVisitor visitor = {
                [&]() {
                    if (r == 0) {
                        std::cout << "---------------------------------" << std::endl;
                    }
                },
                [&]() {
                    switch (r++) {
                    case 1: std::cout << "        STARDATE           " << std::fixed << std::setprecision(1) << *m_time; break;
                    case 2: std::cout << "        CONDITION          " << m_ent->condition(); break;
                    case 3: std::cout << "        QUADRANT           " << m_ent->quadrant()->pos(); break;
                    case 4: std::cout << "        SECTOR             " << m_ent->sector(); break;
                    case 5: std::cout << "        PHOTON TORPEDOES   " << m_ent->torpedoes_remaining(); break;
                    case 6: std::cout << "        TOTAL ENERGY       " << m_ent->total_energy(); break;
                    case 7: std::cout << "        SHIELDS            " << m_ent->shields(); break;
                    case 8: std::cout << "        KLINGONS REMAINING " << m_galaxy.total_klingons(); break;
                    }
                    std::cout << std::endl;
                },
                [&](const Empty&) {
                    std::cout << "    ";
                },
                [&](const Enterprise&) {
                    std::cout << " <*>";
                },
                [&](const Klingon&) {
                    std::cout << " +K+";
                },
                [&](const StarBase&) {
                    std::cout << " >!<";
                },
                [&](const Star&) {
                    std::cout << "  * ";
                }
            };
            m_ent->short_range_scan(visitor);
            std::cout << "---------------------------------" << std::endl;
        }
        catch (ShortRangeSensorsInoperableException&) {
            std::cout << "*** SHORT RANGE SENSORS ARE OUT ***" << std::endl;
        }
    }

    void long_range_scan() {
        try {
            int r = 0;
            QuadrantVisitor visitor = {
                [&]() {
                    if (r++ == 0) {
                        std::cout << "LONG RANGE SCAN FOR QUADRANT " << m_ent->quadrant()->pos() << std::endl;
                    }
                    std::cout << "-------------------" << std::endl;
                },
                [&]() {
                    std::cout << ":" << std::endl;
                },
                [&]() {
                    std::cout << ": *** ";
                },
                [&](Quadrant& quad) {
                    if (quad.visited()) {
                        std::cout << ": " << quad.num_klingons() << quad.num_bases() << quad.num_stars() << " ";
                    }
                    else {
                        std::cout << ": *** ";
                    }
                }
            };
            m_ent->long_range_scan(visitor);
            std::cout << "-------------------" << std::endl;
        }
        catch (LongRangeSensorsInoperableException&) {
            std::cout << "LONG RANGE SENSORS ARE INOPERABLE" << std::endl;
        }
    }

    void fire_phasers() {
        try {
            std::string input;
            m_ent->fire_phasers(
                [&](int available) {
                    std::cout << "PHASERS LOCKED ON TARGET;  " << std::endl;
                    std::cout << "ENERGY AVAILABLE = " << available << " UNITS" << std::endl;
                    std::cout << "NUMBER OF UNITS TO FIRE ";
                    std::getline(std::cin, input);
                    return std::stoi(input);
                }
            );
        }
        catch (PhasersInoperableException&) {
            std::cout << "PHASERS INOPERATIVE" << std::endl;
        }
        catch (std::invalid_argument&) {
            // do nothing
        }
    }

    void fire_torpedoes() {
        try {
            bool missed;
            std::vector<Position> track;
            SectorVisitor visitor = {
                [&]() {}, [&]() {}, [&](const Empty&) {}, [&](const Enterprise&) {},
                [&](const Klingon&) {
                    std::cout << "*** KLINGON DESTROYED ***" << std::endl;
                },
                [&](const StarBase&) {
                    std::cout << "*** STARBASE DESTROYED ***" << std::endl;
                },
                [&](const Star& s) {
                    std::cout << "STAR AT " << s.sector() << " ABSORBED TORPEDO ENERGY." << std::endl;
                }
            };
            bool first = true;
            m_ent->fire_torpedo(
                [&](int min, int max) {
                    std::cout << "PHOTON TORPEDO COURSE (1-9) ";
                    read_cmd();
                    return std::stod(m_cmd);
                },
                [&](const Position& p) {
                    if (first) {
                        std::cout << "TORPEDO TRACK:" << std::endl;
                        first = false;
                    }
                    std::cout << "               " << p << std::endl;
                },
                    visitor,
                    missed
                    );
            if (missed) {
                std::cout << "TORPEDO MISSED" << std::endl;
                short_range_scan();
            }
        }
        catch (TorpedoesExpendedException&) {
            std::cout << "ALL PHOTON TORPEDOES EXPENDED" << std::endl;
        }
        catch (TorpedoesInoperableException&) {
            std::cout << "PHOTON TUBES ARE NOT OPERATIONAL" << std::endl;
        }
        catch (InvalidCourseException&) {
            std::cout << "ENSIGN CHEKOV REPORTS,  'INCORRECT COURSE DATA, SIR!'" << std::endl;
        }
        catch (StarbaseDestroyedException&) {
            std::cout << "STARFLEET COMMAND REVIEWING YOUR RECORD TO CONSIDER" << std::endl;
            std::cout << "COURT MARTIAL!" << std::endl;
        }
        catch (std::invalid_argument&) {
            // do nothing
        }
    }

    void raise_shields() {
        try {
            bool changed = false;
            try {
                std::string m_input;
                bool changed;
                m_ent->raise_shields(
                    [&](int energy_available) {
                        std::cout << "ENERGY AVAILABLE = " << energy_available << std::endl;
                        std::cout << "NUMBER OF UNITS TO SHIELDS ";
                        std::getline(std::cin, m_input);
                        return std::stoi(m_input);
                    },
                    changed
                        );
            }
            catch (InvalidShieldChangeException&) {
                std::cout << "SHIELD CONTROL REPORTS  'THIS IS NOT THE FEDERATION TREASURY.'" << std::endl;
            }
            if (changed) {
                std::cout << "DEFLECTOR CONTROL ROOM REPORT:" << std::endl;
                std::cout << "  'SHIELDS NOW AT " << m_ent->shields() << " UNITS PER YOUR COMMAND.'" << std::endl;
            }
            else {
                std::cout << "<SHIELDS UNCHANGED>" << std::endl;
            }
        }
        catch (ShieldControlInoperableException&) {
            std::cout << "SHIELD CONTROL INOPERABLE" << std::endl;
        }
        catch (std::invalid_argument&) {
            // do nothing
        }
    }

    void damage_control_report() {
        m_ent->repair_damage(
            [&]() {
                std::cout << "DAMAGE CONTROL REPORT NOT AVAILABLE" << std::endl;
            },
            [&](double t) {
                std::cout << std::endl;
                std::cout << "TECHNICIANS STANDING BY TO EFFECT REPAIRS TO YOUR SHIP;" << std::endl;
                std::cout << "ESTIMATED TIME TO REPAIR: " << std::setprecision(1) << t << " STARDATES" << std::endl;
                std::cout << "WILL YOU AUTHORIZE THE REPAIR ORDER (Y/N) ";
                read_cmd();
                return m_cmd == "Y";
            },
                [&](std::vector<std::pair<DamageType, double>>& damage) {
                std::cout << std::endl;
                std::cout << "DEVICE              STATE OF REPAIR" << std::endl;
                for (auto it = damage.begin(); it != damage.end(); it++) {
                    std::cout << std::setw(20) << std::left << it->first << std::setprecision(2) << it->second << std::endl;
                }
            }
            );
    }

    void computer() {
        if (m_ent->computer_damaged()) {
            std::cout << "COMPUTER DISABLED" << std::endl;
        }
        else {
            while (true) {
                try {
                    std::cout << "COMPUTER ACTIVE AND AWAITING COMMAND ";
                    std::getline(std::cin, m_cmd);
                    int cmd = std::stoi(m_cmd);
                    if (cmd < 0) {
                        break;
                    }
                    auto it = m_computer_commands.find(cmd);
                    if (it != m_computer_commands.end()) {
                        it->second();
                        break;
                    }
                }
                catch (std::invalid_argument) {
                    // do nothing
                }
                std::cout << "FUNCTIONS AVAILABLE FROM LIBRARY-COMPUTER:" << std::endl;
                std::cout << "   0 = CUMULATIVE GALACTIC RECORD" << std::endl;
                std::cout << "   1 = STATUS REPORT" << std::endl;
                std::cout << "   2 = PHOTON TORPEDO DATA" << std::endl;
                std::cout << "   3 = STARBASE NAV DATA" << std::endl;
                std::cout << "   4 = DIRECTION/DISTANCE CALCULATOR" << std::endl;
                std::cout << "   5 = GALAXY 'REGION NAME' MAP" << std::endl;
            }
        }
    }

    QuadrantVisitor quadrant_visitor(const std::string& title, int& r, std::function<void(Quadrant & quad)> visit_quadrant) {
        return {
            [&]() {
                if (r++ == 0) {
                    std::cout << title << m_ent->quadrant()->pos() << std::endl;
                    std::cout << std::endl;
                    std::cout << "       1     2     3     4     5     6     7     8" << std::endl;
                }
                std::cout << "     ----- ----- ----- ----- ----- ----- ----- -----" << std::endl;
                std::cout << "   " << r;
            },
            [&]() {
                std::cout << std::endl;
                if (r == 8) {
                    std::cout << "     ----- ----- ----- ----- ----- ----- ----- -----" << std::endl;
                }
            },
            [&]() {},
            visit_quadrant
        };
    }

    void galactic_record() {
        int r = 0;
        auto visitor = quadrant_visitor("        COMPUTER RECORD OF GALAXY FOR QUADRANT ", r, [&](Quadrant& quad) {
            std::cout << "  ";
            if (quad.visited()) {
                std::cout << quad.num_klingons() << quad.num_bases() << quad.num_stars();
            }
            else {
                std::cout << "***";
            }
            std::cout << " ";
            });
        m_galaxy.visit_quadrants(visitor);
    }

    void status_report() {
        int klg = m_galaxy.total_klingons();
        int base = m_galaxy.total_bases();
        std::cout << "   STATUS REPORT:" << std::endl;
        std::cout << "KLINGON" << (klg > 1 ? "S" : "") << " LEFT: " << klg << std::endl;
        std::cout << "MISSION MUST BE COMPLETED IN " << std::setprecision(1) << m_time.deadline() << " STARDATES" << std::endl;
        std::cout << "THE FEDERATION IS MAINTAINING " << base << " STARBASE" << (base > 1 ? "S" : "") << " IN THE GALAXY" << std::endl;
        if (base < 1) {
            std::cout << "YOUR STUPIDITY HAS LEFT YOU ON YOUR OWN IN" << std::endl;
            std::cout << "  THE GALAXY -- YOU HAVE NO STARBASES LEFT!" << std::endl;
        }
        else {
            std::cout << std::endl;
        }
        damage_control_report();
    }

    void torpedo_data() {
        std::vector<std::pair<double, double>> dir_dist;
        m_ent->quadrant()->torpedo_data(dir_dist);
        std::cout << "FROM ENTERPRISE TO KLINGON BATTLE CRUISER" << (dir_dist.size() > 1 ? "S" : "") << std::endl;
        for (auto it = dir_dist.begin(); it != dir_dist.end(); it++) {
            std::cout << "DIRECTION = " << it->first << std::endl;
            std::cout << "DISTANCE = " << it->second << std::endl;
        }
    }

    void starbase_data() {
        try {
            double dir;
            double dist;
            m_ent->quadrant()->starbase_data(dir, dist);
            std::cout << "FROM ENTERPRISE TO STARBASE:" << std::endl;
            std::cout << "DIRECTION = " << dir << std::endl;
            std::cout << "DISTANCE = " << dist << std::endl;
        }
        catch (NoStarbaseException) {
            std::cout << "MR. SPOCK REPORTS,  'SENSORS SHOW NO STARBASES IN THIS  QUADRANT.'" << std::endl;
        }
    }

    void dir_dist_calc() {
        try {
            std::cout << "DIRECTION/DISTANCE CALCULATOR:" << std::endl;
            std::cout << "YOU ARE AT QUADRANT " << m_ent->quadrant()->pos() << " SECTOR " << m_ent->sector() << std::endl;
            std::cout << "PLEASE ENTER" << std::endl;
            std::cout << "  INITIAL COORDINATES (X,Y) ";
            read_cmd();
            Position from = m_cmd;
            std::cout << "  FINAL COORDINATESS (X,Y) ";
            read_cmd();
            Position to = m_cmd;
            std::cout << "DIRECTION = " << from.direction(to) << std::endl;
            std::cout << "DISTANCE = " << from.dist(to) << std::endl;
        }
        catch (std::invalid_argument) {
            // do nothing
        }
    }

    void galactic_map() {
        int r = 0;
        int c = 0;
        auto visitor = quadrant_visitor("                        THE GALAXY", r, [&](Quadrant& quad) {
            if (++c % 4 == 0) {
                int w = 24;
                int n = int(quad.region().size());
                int left = (w - n) / 2;
                int right = w - n - left;
                std::cout << " " << std::setw(left) << " " << std::setw(n) << quad.region() << std::setw(right) << " ";
            }
            });
        m_galaxy.visit_quadrants(visitor);
    }

    void resign() {
        m_resign = true;
    }

    virtual void entering_quadrant_mission_start(Quadrant& quad) {
        std::cout << "YOUR MISSION BEGINS WITH YOUR STARSHIP LOCATED" << std::endl;
        std::cout << "IN THE GALACTIC QUADRANT, '" << quad.name() << "'." << std::endl;
        quadrant_status(quad);
    }

    virtual void entering_quadrant(Quadrant& quad) {
        std::cout << "NOW ENTERING " << quad.name() << " QUADRANT . . ." << std::endl;
        quadrant_status(quad);
    }

    void quadrant_status(Quadrant& quad) {
        if (quad.num_klingons() > 0) {
            std::cout << "COMBAT AREA      CONDITION RED" << std::endl;
            if (m_ent->shields_low()) {
                std::cout << "   SHIELDS DANGEROUSLY LOW" << std::endl;
            }
        }
    }

    virtual void shields_lowered_for_docking() {
        std::cout << "SHIELDS DROPPED FOR DOCKING PURPOSES" << std::endl;
    }

    virtual void starbase_shields_absorbed_hit() {
        std::cout << "STARBASE SHIELDS PROTECT THE ENTERPRISE" << std::endl;
    }

    virtual void enterprise_hit_from_sector(int hit, const Position& sector) {
        std::cout << hit << " UNIT HIT ON ENTERPRISE FROM SECTOR " << sector << std::endl;
    }

    virtual void enterprise_shields_down_to(int shields) {
        std::cout << "      <SHIELDS DOWN TO " << shields << " UNITS>" << std::endl;
    }

    virtual void enterprise_damaged_by_hit(const DamageType& dt) {
        std::cout << "DAMAGE CONTROL REPORTS " << dt << " DAMAGED BY THE HIT'" << std::endl;
    }

    virtual void enterprise_damage_repaired(const std::vector<std::pair<DamageType, RepairStatus>>& status) {
        std::cout << "DAMAGE CONTROL REPORT:  " << std::endl;
        for (auto it = status.begin(); it != status.end(); it++) {
            switch (it->second) {
            case RepairStatus::COMPLETED: std::cout << "        " << it->first << " REPAIR COMPLETED."; break;
            case RepairStatus::DAMAGED: std::cout << it->first << " DAMAGED"; break;
            case RepairStatus::IMPROVED: std::cout << it->first << " STATE OF REPAIR IMPROVED"; break;
            }
            std::cout << std::endl;
        }
    }

    virtual void engines_shut_down_for_bad_navigation(const Position& sector) {
        std::cout << "WARP ENGINES SHUT DOWN AT SECTOR " << sector << " DUE TO BAD NAVAGATION" << std::endl;
    }

    virtual void maneuver_energy_provided_by_shields() {
        std::cout << "SHIELD CONTROL SUPPLIES ENERGY TO COMPLETE THE MANEUVER." << std::endl;
    }

    virtual void engines_shut_down_for_perimeter_breach(const Position& quadrant, const Position& sector) {
        std::cout << "LT. UHURA REPORTS MESSAGE FROM STARFLEET COMMAND:" << std::endl;
        std::cout << "  'PERMISSION TO ATTEMPT CROSSING OF GALACTIC PERIMETER" << std::endl;
        std::cout << "  IS HEREBY *DENIED*.  SHUT DOWN YOUR ENGINES.'" << std::endl;
        std::cout << "CHIEF ENGINEER SCOTT REPORTS  'WARP ENGINES SHUT DOWN" << std::endl;
        std::cout << "  AT SECTOR " << sector << " OF QUADRANT " << quadrant << ".'" << std::endl;
    }

    virtual void firing_phasers_damaged_computer() {
        std::cout << "COMPUTER FAILURE HAMPERS ACCURACY" << std::endl;
    }

    virtual void klingon_took_no_damage(const Position& sector) {
        std::cout << "SENSORS SHOW NO DAMAGE TO ENEMY AT " << sector << std::endl;
    }

    virtual void klingon_took_damage(int hit, const Position& sector) {
        std::cout << hit << " UNIT HIT ON KLINGON AT SECTOR " << sector << std::endl;
    }

    virtual void klingon_destroyed() {
        std::cout << "*** KLINGON DESTROYED ***" << std::endl;
    }

    virtual void klingon_shields_remaining(int shields) {
        std::cout << "   (SENSORS SHOW " << shields << " UNITS REMAINING)" << std::endl;
    }
};

int main() {
    bool keep_playing;
    do {
        StarTrek game;
        game.run(keep_playing);
    } while (keep_playing);
}