using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Game.Entity.NativeWeb
{
  public  class ReceiveRankAward
    {
        public int UserID { get; set; }
        public long Score { get; set; }
        public int Diamond { get; set; }
        public int AwardTicket { get; set; }
    }
}
