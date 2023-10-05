using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Game.Entity.Platform
{
    public partial class HallBattleInfo
    {
        public int KindID { get; set; }
        public string KindName { get; set; }
        public int RoomID { get; set; }
        public int BaseScore { get; set; }
        public DateTime StartTime { get; set; }
        public int RoomHostID { get; set; }
        public int Score { get; set; }
        public int GameID { get; set; }
        public string NickName { get; set; }
        public int PlayBackCode { get; set; }
        public string GUID { get; set; }
        public int ChairID { get; set; }
        public List<HallGamesNum> GamesNum { get; set; }
    }

    public partial class HallGamesNum
    {
        public int GamesNum { get; set; }
        public List<HalllBattleUser> BattleUser { get; set; }
    }

    public partial class HalllBattleUser
    {
        public string NickName { get; set; }
        public int GameID { get; set; }
        public int Score { get; set; }
        public string VideoNumber { get; set; }
    }

    public partial class HalllScoreInfo
    {
        public int KindID { get; set; }
        public string KindName { get; set; }
        public DateTime GameTime { get; set; }
        public int UserCount{ get; set; }
        public int Score { get; set; }
    }

}
