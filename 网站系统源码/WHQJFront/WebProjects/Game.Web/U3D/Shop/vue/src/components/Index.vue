<template>
  <main>
    <div class="ui-productPage">
      <div class="ui-nav">
         <ul>
         
            <li  v-for="(types,index) in awardtype" :key="types.TypeID" v-bind:class="types.ClassName">
                <a  href="javascript:;" v-on:click="changeData(types.TypeID,index)" >{{types.TypeName}}</a>
            </li>
        </ul>
      </div> 

      <div class="ui-product-content">
        <ul class="ui-list fn-clear">
          <li v-for="(award,index) in awardlist" :key="award.AwardID" :class="(index+1)%4==0?activeClass:''">
                <img v-bind:src="award.AwardImg" style="width:85%" class="listImg" />
                <!-- <p><span style="color: #fff;">{{award.AwardName}}</span></p> -->
                 <a href="javascript:;" v-on:click="detail(award.AwardID,award.AwardType,award.AwardName,award.UnitPrice)"><span>{{award.UnitPrice}}</span></a>
            </li>
        </ul>
      <!-- 弹窗 -->
    <div class="ui-dialog" v-show = "show" >
      <i class="ui-dialog-close" v-on:click="cancel"></i>
        <!-- tainer1 -->
        <div class="ui-dialog-tainer1" v-show = "content1show">
            <div class="ui-tainer1-content">
              <p><b>兑换奖品：</b><span>{{this.Name}}</span></p>
              <p><b>兑奖数量：</b><span class="ui-amount"><a href="javascript:;" class="ui-reduce fn-left" v-on:click="addbuynum(-1)"></a><input readonly="true"  v-model="this.BuyNum" /><a href="javascript:;" class="ui-plus fn-right" v-on:click="addbuynum(1)"></a></span></p>
              <p><b>价格：</b><span>{{this.sumprice}}奖券</span></p>
            </div>
            <div class="ui-dialog-btn">
              <a href="javascript:;" class="ui-dialog-cancel" v-on:click="cancel"></a><a href="javascript:;" class="ui-dialog-confirm" v-on:click="exchange"></a>
            </div>
          </div>

        <!-- tainer1-end -->


        <!-- tainer2 -->
          <div class="ui-dialog-tainer2" v-show = "content2show">
            <div class="ui-tainer2-content">
             <p>是否消耗&nbsp;<span>{{this.sumprice}}奖券</span>兑换&nbsp;<span>{{this.Name+"x"+this.BuyNum}} </span></p>
            </div>
            <div class="ui-dialog-btn">
              <a href="javascript:;" class="ui-dialog-cancel" v-on:click="cancel"></a><a href="javascript:;" class="ui-dialog-confirm" id="toOrderPage"  v-on:click="btn"></a>
            </div>
          </div>

        <!-- tainer2-end -->


        <!-- tainer4 -->
          <div class="ui-dialog-tainer4" v-show = "content4show">
            <div class="ui-tainer4-content">
              <p>{{this.msg}}</p>
            </div>
            <div class="ui-dialog-btn">
              <a href="javascript:;" class="ui-dialog-confirm" v-on:click="cancel"></a>
            </div>
          </div>
        <!-- tainer4-end -->
      </div>
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
        this.cacheuserid = sessionStorage.getItem("cacheid");
        if (this.$route.params.typeid > 0) {
          this.axios
            .get(
              "/U3D/Shop/WHShop.ashx?action=getshophome&typeid=" +
             this.$route.params.typeid+"&userid="+cacheid
            )
            .then(response => {
              var data = response.data.data;
              this.awardtype = data.type;
              this.awardlist = data.list;
               this.awardlist.forEach(element=>{
              if(element.AwardName.length>8){
                element.AwardName=element.AwardName.substring(0,7)+'...'
              }
            })
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
            .get("/U3D/Shop/WHShop.ashx?action=getshophome&userid="+this.cacheuserid)
            .then(response => {
              var data = response.data.data;
              this.awardtype = data.type;
              this.awardlist = data.list;
               this.awardlist.forEach(element=>{
              if(element.AwardName.length>8){
                element.AwardName=element.AwardName.substring(0,7)+'...'
              }
            })
              this.typeid = this.awardlist[0].AwardType;
             this.AwardTicket=data.AwardTicket;
              // this.awardtype = [{"TypeID":1,"TypeName":"111111","ClassName":"11111"},{"TypeID":2,"TypeName":"22222","ClassName":"11111"},{"TypeID":3,"TypeName":"33333","ClassName":"11111"}];
               this.awardtype[0].ClassName = "active";
              //console.log(this.awardtype);
            });
        }
      }
    });
  },
  methods: {
    changeData: function(e,index) {
      if(this.isclick){
    this.axios
        .get(`/U3D/Shop/WHShop.ashx?action=getawardinfolist&typeid=${e}`)
        .then(response => {
          this.awardlist = response.data.data.list;
            this.awardlist.forEach(element=>{
              if(element.AwardName.length>8){
                element.AwardName=element.AwardName.substring(0,7)+'...'
              }
            })

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
      console.log(id, type, name, price);

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
            "/U3D/Shop/WHShop.ashx?action=buyaward&userid=" +
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
  },
  updated(){
    var listImg = document.querySelectorAll('.listImg');
    for(var i=0; i<listImg.length;i++){
      listImg[i].height = listImg[i].width;
    }   
  }
};
</script>
