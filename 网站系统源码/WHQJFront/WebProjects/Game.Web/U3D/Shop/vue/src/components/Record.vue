<template>
  <main>
 <div class="ui-recordPage">
      <p>兑换记录</p>
      <div class="ui-record-content">
        <i class="ui-record-close" v-on:click="cancel"></i>
        <ul class="ui-record-list">
          <li v-for="record in list" :key="record.OrderID">
            <p><b>订单详情：</b><span>{{record.OrderID}}</span></p>
            <p><b>时间：</b><span>{{record.CollectDate}}</span></p>
            <p><b>物品：</b><span>{{record.AwardName}}</span></p>
            <p><b>数量：</b><span>{{record.BuyNum}}</span></p>
          </li>
        </ul>
          <p  class="ui-record-title tips" v-show="this.show">暂无兑换记录!</p>
      </div>
    </div>


  </main>
</template>

<script>
    export default {
        data: function () {
            return {
                list:[],
                show:false
            }
        },
        mounted: function () {
            this.$nextTick(function () {
              var userid = this.$route.query.userid;
                var cacheid = userid;
                if (cacheid == null) {
                    this.$router.push({ path: '/OverTime', name: 'OverTime' });
                } else {
                    this.axios.get('/U3D/Shop/WHShop.ashx?action=getawardrecord&uid=' + cacheid).then((response) => {
                        this.list = response.data.data.list;
                            if(this.list.length<=0){
                              this.show=true;
                            }
                    });   
                  
                }
               
            });

        },
         methods:{
           cancel: function () {
                this.$router.push({ path: '/Index', name: 'Index' ,query:{userid:this.$route.query.userid}});
            }
        }
      
    }
</script>
<style >
 @import '../assets/css/style.css';

 .tips{
    padding-top: 20%;
    text-align: center;
    color: #ffe6a2;
    font-weight: 900;
    font-size: 16px;
    margin-bottom: .6%;
 }
 
</style>
