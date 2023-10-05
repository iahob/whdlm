<template>
  <main>
  <div class="ui-orderPage" v-show="ordershow">
      <p>订单详情</p>
      <div class="ui-order-content">
        <div><b>商品名称：</b><span>{{AwardName}}</span></div>
        <div><b>数量：</b><span>{{BuyNum}}</span></div>
        <div><b>发货方：</b><span>京东/天猫</span></div>
        <div class="ui-accept-msg"><b>收件人信息：</b><span v-show="writeshow">无，点击&nbsp;<a href="javascript:;" id="write" v-on:click="writeaccept">填写地址</a>&nbsp;完善个人信息</span><a
             id="setting" v-show="settingshow" v-on:click="writeaccept">修改</a>
        </div>
        <div class="ui-order-address" v-show="IsShow">
          <div><b>姓名：</b><input v-model="Compellation" readonly="true" /></div>
          <div><b>联系方式：</b><input v-model="MobilePhone" readonly="true" /></div>
          <div class="ui-order-address-last"><b>收货地址：</b><input v-model="DetailAddress1" readonly="true" /></div>
        </div>
        <div class="ui-order-bottom">
          <p>提示：您兑换的实物商品将在一周内（不含节假日）发出</p>
          <a href="javascript:;" class="ui-dialog-cancel" v-on:click="canceltt"></a><a href="javascript:;" class="ui-dialog-confirm" v-on:click="order"></a>
        </div>
      </div>
    </div>

 <!-- 收件人信息页面 -->
      <div class="ui-receivePage" v-show="acceptshow">
      <p>收件人信息</p>
      <div class="ui-receive-content">
        <div class="ui-receive-msg"><b>姓名：</b><input v-model="Compellation" /></div>
        <div class="ui-receive-msg"><b>联系方式：</b><input v-model="MobilePhone" /></div>
        <div class="ui-receive-msg">
          <b>地址：</b>
            <v-distpicker class="distpicker-address-wrappers" id="distpicker"   :province="select.province" @province="changeProvince"
                  :city="select.city" @city="changeCity"
                  :area="select.area" @area="changeArea" ></v-distpicker>
        </div>
        <div class="ui-receive-msg"><b>详细地址：</b><input v-model="DetailAddress" class="ui-address" /></div>
        <div class="ui-receive-bottom">
          <a href="javascript:;" class="ui-dialog-cancel" v-on:click="cancel"></a><a href="javascript:;" class="ui-dialog-confirm"  v-show="addshow" v-on:click="addinfo"></a><a href="javascript:;" class="ui-dialog-confirm"  v-show="updateshow" v-on:click="updateinfo"></a>
        </div>
      </div>
    </div>
    <!-- 收件人-end -->

<!-- 弹窗 -->
    
 <div class="ui-dialog" v-show = "show" >
      <i class="ui-dialog-close" v-on:click="closedialog"></i>
        <!-- tainer4 -->
        <div class="ui-dialog-tainer4" >
            <div class="ui-tainer4-content">
              <p>{{this.msg}}</p>
            </div>
            <div class="ui-dialog-btn">
              <a href="javascript:;" class="ui-dialog-confirm" v-on:click="cancel"></a>
            </div>
          </div>
        <!-- tainer4-end -->
      </div>

  </main>
</template>
<style>
 @import '../assets/css/style.css';
#distpicker {
  color: #9caebf;
  display: inline;
  }
#distpicker select {
    width: 90px;
    padding: .5rem .75rem;
    height: 30px;
    font-size: 1rem;
    line-height: 1.25;
    color: #fff;
    background-color: #21282d;
    background-image: none;
    -webkit-background-clip: padding-box;
    background-clip: padding-box;
    border: 1px solid rgba(0,0,0,.15);
    border-radius: .25rem;
    -webkit-transition: border-color ease-in-out .15s,-webkit-box-shadow ease-in-out .15s;
    transition: border-color ease-in-out .15s,-webkit-box-shadow ease-in-out .15s;
    -o-transition: border-color ease-in-out .15s,box-shadow ease-in-out .15s;
    transition: border-color ease-in-out .15s,box-shadow ease-in-out .15s;
    transition: border-color ease-in-out .15s,box-shadow ease-in-out .15s,-webkit-box-shadow ease-in-out .15s;
    }
#distpicker  select  option {
      font-weight: normal;
      display: block;
      white-space: pre;
      min-height: 1.2em;
      padding: 0px 2px 1px;
    }
</style>
<script>
import VDistpicker from 'v-distpicker'

    export default {
      components: { VDistpicker },
        data: function () {
            return {
               select: { province: '广东省', city: '广州市', area: '海珠区' },
                AwardName: '',
                Compellation: '',
                MobilePhone: '',
                DetailAddress: '',
                DetailAddress1: '',
                BuyNum: 0,
                AwardID:0,
                TipInfo: '',
                cacheuserid:0,
                IsShow: false,
                show:false,
                acceptshow:false,
                ordershow:true,
                writeshow:true,
                settingshow:false,
                addshow:true,
                updateshow:false,
                msg:"",
                rs:0,
                oldCompellation:'',
                oldDetailAddress:'',
                oldMobilePhone:'',
                typeid:0,
                AwardTicket:0,
                sumUnitPrice:0

            }
        },
        mounted: function () {
            this.$nextTick(function () {            
                var cacheid = sessionStorage.getItem('cacheid');
                if (cacheid == null) {
                    this.$router.push({ path: '/OverTime', name: 'OverTime' });
                } else {
                  this.cacheuserid=cacheid;
                    var id = this.$route.params.id;
                    this.BuyNum=this.$route.params.num;
                    this.axios.get('/U3D/Shop/WHShop.ashx?action=getorderawardInfo&id=' + id + '&uid=' + cacheid).then((response) => {
                        var data = response.data.data;
                        this.AwardName = data.award[0].AwardName;
                        this.AwardTicket=data.AwardTicket;
                        this.AwardID=data.award[0].AwardID;
                        this.typeid=data.award[0].AwardType;
                        this.sumUnitPrice=data.award[0].UnitPrice*this.BuyNum;
                        if (data.uinfo.length > 0) {
                            
                           this.IsShow = true;
                            this.writeshow=false;
                            this.settingshow=true;
                            this.addshow=false;
                            this.updateshow=true;
                            this.Compellation = data.uinfo[0].Compellation;
                            this.MobilePhone = data.uinfo[0].MobilePhone;
                            this.DetailAddress = data.uinfo[0].DetailAddress;
                            this.select.province=data.uinfo[0].Province;
                            this.select.city=data.uinfo[0].City;
                            this.select.area=data.uinfo[0].Area;
                            this.DetailAddress1=this.select.province+this.select.city+this.select.area+this.DetailAddress;
                            this.oldCompellation=this.Compellation;
                            this.oldMobilePhone=this.MobilePhone;
                            this.oldDetailAddress=this.DetailAddress;
                            

                        } else {
                            this.IsShow = false;
                            this.writeshow=true;
                           this.settingshow=false;
                            this.addshow=true;
                            this.updateshow=false;
                        }
                    });
                }
            });
        },
        methods: {
           changeProvince(data) {
                this.select.province = data.value;
                 
            },
            changeCity(data) {
                this.select.city = data.value;
                
            },
            changeArea(data) {
                this.select.area = data.value;
            },     
            addinfo: function () {
               
                this.axios
          .get("/U3D/Shop/WHShop.ashx?action=setawarduser&userid="+ 
               this.cacheuserid+"&settype=insert&name="+
               this.Compellation+"&phone="+
               this.MobilePhone+"&province="+
               this.select.province+ "&city="+
               this.select.city+"&area="+
               this.select.area+"&address="+
               this.DetailAddress
          )
          .then(response => {
            var data = response.data.data;
             this.msg=response.data.msg;
              if(data.rs==1){
               this.rs=-1;
              this.oldMobilePhone= this.MobilePhone;
              this.oldCompellation=this.Compellation;
              this.oldDetailAddress=this.DetailAddress;
               this.DetailAddress1=this.select.province+this.select.city+this.select.area+this.DetailAddress;
             this.IsShow=true;
             this.writeshow=false;
             this.settingshow=true;   
             }
              else if(data.valid==false){
              this.rs=-2;
            }
             this.show=true;
            
          });

            },
            updateinfo: function (name, phone, address) {
             
      this.axios
          .get("/U3D/Shop/WHShop.ashx?action=setawarduser&userid="+ 
               this.cacheuserid+"&settype=update&name="+
               this.Compellation+"&phone="+
               this.MobilePhone+"&province="+
               this.select.province+ "&city="+
               this.select.city+"&area="+
               this.select.area+"&address="+
               this.DetailAddress
          )
          .then(response => {
            var data = response.data.data;
              this.msg=response.data.msg;
              if(data.rs==1){
               this.rs=-1;
              this.DetailAddress1=this.select.province+this.select.city+this.select.area+this.DetailAddress; 
             }
            else if(data.valid==false){
              this.rs=-2;
            }
              this.show=true;
              
          });
            },
            order: function () {
             if(this.IsShow==true || this.AwardTicket<this.sumUnitPrice){
          //地址信息存在，开始下单
           this.axios
          .get("/U3D/Shop/WHShop.ashx?action=buyaward&userid="+ this.cacheuserid+"&awardID="+this.AwardID+"&counts="+this.BuyNum)
          .then(response => {
            var data = response.data.data;
             this.msg=response.data.msg;
             this.rs=data.rs;
             this.show=true;
             
          });
             }
             else{ 
               this.msg="收件人信息未填写，兑换失败";
               this.show=true;
             }

            },
            cancel: function () {
              if(this.rs>0){
                  this.$router.push({ path: '/Index', name: 'Index' });
              }
              else if(this.rs==-1){
                 this.acceptshow=false;
                 this.ordershow=true;
                 this.show=false;
                 this.rs=0;
              } 
              else if(this.rs==-2){
                  this.show=false;
                 this.rs=0;
              }
          else{
              this.MobilePhone=this.oldMobilePhone;
              this.Compellation=this.oldCompellation;
              this.DetailAddress=this.oldDetailAddress;
              this.acceptshow=false;
              this.ordershow=true;
              this.show=false;
              }
            },
          canceltt: function () {
              this.$router.push({ path: '/Index', name: 'Index', params: { typeid:this.typeid } });
            },
            closedialog:function(){
              this.show=false;
            },
            writeaccept:function(){
              this.ordershow=false;
              this.acceptshow=true;

            }
        }
    }

</script>


<!-- Add "scoped" attribute to limit CSS to this component only -->

