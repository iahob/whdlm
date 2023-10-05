using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Game.Entity.Accounts
{
    public class UserInfo
    {
        public int UserID { get; set; }
        public int GameID { get; set; }
        public int SpreaderID { get; set; }
        public string Accounts { get; set; }
        public string NickName { get; set; }
        public int FaceID { get; set; }
        public int CustomID { get; set; }
        public int AgentID { get; set; }
    }
}
