<template>

  <main>
    <div class="ui-product-page">
      <div class="ui-nav">
         <ul>
         
            <li  v-for="(types,index) in awardtype" :key="types.TypeID" v-bind:class="types.ClassName">
                <a  href="javascript:;" v-on:click="changeData(types.TypeID,index)" >{{types.TypeName}}</a>
            </li>
        </ul>
      </div> 
      <div class="ui-content">
        <ul class="ui-list fn-left">
          <li v-for="(award,index) in awardlist" :key="award.AwardID" :class="(index+1)%4==0?activeClass:''">
                <img v-bind:src="award.AwardImg" class="listImg" />
                <p><span>{{award.AwardName}}</span></p>
                 <a href="javascript:;" v-on:click="detail(award.AwardID,award.AwardType,award.AwardName,award.UnitPrice)"><span>{{award.UnitPrice}}</span></a>
            </li>
        </ul>
        <div class="ui-record-btn fn-right">
         <router-link to="/Record">
            <img src="../assets/images/record_icon.png"/>
         </router-link>
        </div>
      </div>
      
      <!-- 弹窗 -->
      <div class="ui-mask"></div>
    <div class="ui-dialog" v-show = "show" >
    <i class="ui-dialog-close fn-right" v-on:click="cancel"></i>
        <!-- tainer1 -->
        <div class="ui-dialog-content1" v-show = "content1show">
          <p><b>兑奖物品：</b><span>{{this.Name}}</span></p>
          <p><b>兑奖数量：</b><span class="ui-amount"><a href="javascript:;" class="ui-reduce fn-left" v-on:click="addbuynum(-1)"></a><input readonly="true"  v-model="this.BuyNum" /><a href="javascript:;" class="ui-plus fn-right" v-on:click="addbuynum(1)"></a></span></p>
          <p class="nomargin-bottom"><b>价格：</b><span>{{this.sumprice}}奖券</span></p>
          <div class="ui-dialog-btn">
            <a href="javascript:;" class="ui-dialog-exchange" v-on:click="exchange"></a>
          </div>
        </div>
        <!-- tainer1-end -->

        <!-- tainer2 -->
        <div class="ui-dialog-content2" v-show = "content2show">
          <p>是否消耗&nbsp;<span>{{this.sumprice}}奖券</span>兑换&nbsp;<span>{{this.Name+"x"+this.BuyNum}} </span></p>
          <div class="ui-dialog-btn">
            <a href="javascript:;" class="ui-dialog-cancel" v-on:click="cancel"></a><a href="javascript:;" id="toOrderPage" class="ui-dialog-definite" v-on:click="btn"></a>
          </div>
        </div>
        <!-- tainer2-end -->

        <!-- tainer3 -->
        <div class="ui-dialog-content3" v-show = "content3show">
          <p>兑换成功&nbsp;！实物商品请注意查收</p>
          <div class="ui-dialog-btn">
            <a href="javascript:;" class="ui-dialog-confirm"></a>
          </div>
        </div>
        <!-- tainer3-end -->


        <!-- tainer4 -->
        <div class="ui-dialog-content4" v-show = "content4show">
          <p> {{this.msg}} </p>
          <div class="ui-dialog-btn">
            <a href="javascript:;" class="ui-dialog-confirm" v-on:click="cancel"></a>
          </div>
        </div>
        <!-- tainer4-end -->
      </div>
    </div>
  </main>
</template>

<style >
@import "../assets/css/style.css";
a {
  text-decoration: none;
  color: #fff;
}
</style>

<script>
export default {
  data: function() {
    return {
      awardtype: [],
      isclick:true,
      awardlist: [],
      BuyNum: 1,
      price: 0,
      sumprice: 0,
      Name: "",
      show: false,
      content1show: true,
      content2show: false,
      content3show: false,
      content4show: false,
      typeid: 0,
      AwardID: 0,
      msg: "",
      cacheuserid: 0,
      nomargin: false,
      activeClass: "nomargin-right",
      AwardTicket:0
    };
  },
  mounted: function() {
    this.$nextTick(function() {
      var userid = this.$route.query.userid;
      var cacheid = sessionStorage.getItem("cacheid");
      if (userid == undefined && cacheid == null) {
        this.$router.push({ path: "/OverTime", name: "OverTime" });
      } else {
        if (userid == undefined) {
          sessionStorage.setItem("cacheid", cacheid);
        } else {
          sessionStorage.setItem("cacheid", userid);
        }

        cacheid = sessionStorage.getItem("cacheid");
        this.cacheuserid=cacheid;
        console.log(this.cacheuserid);
        if (this.$route.params.typeid > 0) {
          this.axios
            .get(
              "/Shop/WHShop.ashx?action=getshophome&typeid=" +
             this.$route.params.typeid+"&userid="+cacheid
            )
            .then(response => {
              var data = response.data.data;
              this.awardtype = data.type;
              this.awardlist = data.list;
              this.typeid = this.awardlist[0].AwardType;
              this.AwardTicket=data.AwardTicket;
           this.awardtype.forEach(element => {
             //console.log(element.TypeID,this.awardlist[0].AwardType);
            if(element.TypeID==this.$route.params.typeid){
                       
                      element.ClassName="active";
            }
              });
              //console.log(this.typeid);
            });
        } else {
          this.axios
            .get("/Shop/WHShop.ashx?action=getshophome&userid="+cacheid)
            .then(response => {
              var data = response.data.data;
              this.awardtype = data.type;
              this.awardlist = data.list;
              this.typeid = this.awardlist[0].AwardType;
             this.AwardTicket=data.AwardTicket;
             this.awardtype[0].ClassName = "active";
            });
        }
      }
    });
  },
  methods: {
    changeData: function(e,index) {
      if(this.isclick){
    this.axios
        .get(`/Shop/WHShop.ashx?action=getawardinfolist&typeid=${e}`)
        .then(response => {
          this.awardlist = response.data.data.list;
          this.typeid = this.awardlist[0].AwardType;
          this.awardtype.forEach(element => {  
            element.ClassName="";
          });
          this.awardtype[index].ClassName = "active";
        
         // console.log(this.awardtype);
        });
      }
     
    },
    cancel: function() {
      this.show = false;
      this.content4show = false;
      this.content2show = false;
      this.content1show = true;
      this.msg = "";
      this.BuyNum = 1;
      this.isclick=true;
    },
    detail: function(id, type, name, price) {
      //console.log(id, type, name, price);

      var self = this;
      this.price = price;
      this.sumprice = this.BuyNum * price;
      this.Name = name;
      this.show = true;
      this.typeid = type;
      this.AwardID = id;
      this.isclick=false;
    },
    addbuynum: function(nums) {
      if (nums < 0 && this.BuyNum > 1) {
        this.BuyNum = this.BuyNum + nums;
        this.sumprice = this.price * this.BuyNum;
      } else if (nums > 0 && this.BuyNum >= 0) {
        this.BuyNum = this.BuyNum + nums;
        this.sumprice = this.price * this.BuyNum;
      }
    },
    exchange: function() {
      this.content2show = true;
      this.content1show = false;
    },
    btn: function() {
      if (this.typeid == 1) {
        this.axios
          .get(
            "/Shop/WHShop.ashx?action=buyaward&userid=" +
              this.cacheuserid +
              "&awardID=" +
              this.AwardID +
              "&counts=" +
              this.BuyNum
          )
          .then(response => {
            var data = response.data.data;
            this.msg = response.data.msg;
          });
        this.show = true;
        this.content1show = false;
        this.content2show = false;
        this.content4show = true;
        this.isclick=false;
      } else {
            if(this.AwardTicket<this.sumprice){
                  this.msg='很抱歉！您的奖券数不足，不能兑换该奖品!'
                 this.content2show=false;
                  this.content4show = true;
            }
          else{
            this.$router.push({
          path: "/Order",
          name: "Order",
          params: { id: this.AwardID, num: this.BuyNum }
        });
          }

        
      }
    }
  }
  // updated(){
  //   var listImg = document.querySelectorAll('.listImg');
  //   for(var i=0; i<listImg.length;i++){
  //     listImg[i].height = listImg[i].width;
  //   }   
  // }
};
</script>
