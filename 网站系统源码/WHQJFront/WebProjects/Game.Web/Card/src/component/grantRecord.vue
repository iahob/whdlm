<template>
  <div class="ui-main ui-reward">
    <top :nav="nav"></top>
    <div class="vue-custom-tab">
      <div class="vue-tab-nav">
        <div class="ui-search">
            <div>
              <datepicker class="ui-datepicker" :language="zh" :format='formatDate' v-model="startDate"></datepicker> - <datepicker class="ui-datepicker" :language="zh" :format="formatDate" v-model="endDate"></datepicker>
            </div>
            <input type="text" v-model="search" placeholder="玩家ID/昵称"/>
            <input type="submit" @click="query" value="查询"/>
        </div>
        <div class="ui-reward-title">
          <p>{{labels[type]}}详情</p>
          <a v-show="total!=null">查询业绩汇总 ：<span v-bind="total"></span></a>
        </div>
      </div>
      <div class="ui-panel vue-tab-content">
        <ui-table :thead="theadObj[type]" :datas="record" :type="type" :isPull="true" :upload="upload" :download="download" />
      </div>
    </div>
  </div>
</template>
<script>
import top from "./common/top";
import UiTable from "./table/vue-table";
import { getRecord } from "../fetch/fetch";
import Datepicker from "vuejs-datepicker";
import { zh } from "vuejs-datepicker/dist/locale";
import moment from "moment";

export default {
  name: "grant",
  props: {
    nav: String,
    type:String
  },
  components: { top, UiTable, Datepicker },
  data: function() {
    return {
      zh,
      startDate: new Date(new Date().getFullYear(), new Date().getMonth(), 1),
      endDate: new Date(
        new Date().getFullYear(),
        new Date().getMonth(),
        new Date().getDate()
      ),
      pageSize: 15,
      search:'',
      total: null,
      record: [],
      pages: 0,
      curPage: 1,
      disabled: false,
      labels: {
        diamond:"赠送钻石",
        gold: "赠送金币"
      },
      theadObj: {
        diamond: [
          {
            key: "CollectDate",
            title: "赠送时间"
          },
          {
            key: "NickName",
            title: "赠送玩家"
          },
          {
            key: "ReceiveNickName",
            title: "被赠玩家"
          },
          /*{
            key: "SourceBefore",
            title: "赠送玩家原值"
          },
          {
            key: "TargetBefore",
            title: "被赠玩家原值"
          },*/
          {
            key: "Amount",
            title: "赠送数值"
          }
        ],
        gold: [
          {
            key: "CollectDate",
            title: "赠送时间"
          },
          {
            key: "NickName",
            title: "赠送玩家"
          },
          {
            key: "ReceiveNickName",
            title: "被赠玩家"
          },
          /*{
            key: "SourceBefore",
            title: "赠送玩家原值"
          },
          {
            key: "TargetBefore",
            title: "被赠玩家原值"
          },*/
          {
            key: "Amount",
            title: "赠送数值"
          }
        ]
      }
    };
  },
  created() {
    this.fetchData();
  },
  methods: {
    formatDate (date) {
      return date ? moment(date).format("YYYY-MM-DD") : moment().format('YYYY-MM-DD');
    },
    query(){
      this.curPage=1;
      this.fetchData();
    },
    // 每次改变记录类型都从第一页开始
    changeTab(value) {
      this.curRecord = value;
      this.curPage = 1;
      this.fetchData();
    },
    // 根据页数获取数据
    fetchData: function() {
      let params = {
        token: localStorage.getItem("token"),
        query:this.search,
        startdate:this.formatDate(this.startDate),
        endDate:this.formatDate(this.endDate),
        type: this.type,
        pagesize: this.pageSize,
        pageindex: this.curPage
      };
      getRecord(params, data => {
        // 没有数据时造一个空数据以避免报错（table-body）
        this.record = data.record || [];
        this.pages = data.pageCount;
      });
    },
    // 上拉操作
    upload: function(loaded) {
      this.curPage++;
      this.curPage = this.curPage > this.pages ? this.pages : this.curPage;
      this.fetchData();
      loaded("done");
    },
    // 下拉操作
    download: function(loaded) {
      this.curPage--;
      this.curPage = this.curPage < 1 ? 1 : this.curPage;
      this.fetchData();
      loaded("done");
    }
  }
};
</script>
<style scoped>
.vue-tab-nav {
  width: 96%;
  margin: 0 auto;
  position: relative;
}
.ui-reward .vue-tab-nav li {
  display: inline-block;
  margin: 0.14rem 0rem 0.14rem 1%;
}
.ui-search {
  margin-top: 0.2rem;
  padding-bottom: 0.2rem;
  border-bottom: 1px solid #dedfe0;
  text-align: center;
}
.vdp-datepicker{
  display: inline-block;
  position: static;
}
.vdp-datepicker input {
  font-size: 0.3rem;
  height: 0.6rem;
  line-height: 0.6rem;
  width: 2.3rem;
  background: #f7f7f7;
  font-family: "Microsoft-YaHei";
}
input[type="date"]::-webkit-inner-spin-button {
  visibility: hidden;
}
input[type="date"]::-webkit-clear-button {
  display: none;
}
.ui-search input[type="text"] {
  width: 4.2rem;
  height: 0.32rem;
  padding-top: 0.18rem;
  margin-top: 0.18rem;
  border-bottom: 1px solid #999ca0;
  background: #f7f7f7;
  font-size: 0.32rem;
}
.ui-search input[type="submit"] {
  width: 1rem;
  height: 0.5rem;
  line-height: 0.5rem;
  font-size: 0.32rem;
  margin-top: 0.26rem;
}
.ui-search > div {
  margin: 0 auto;
  width: 5.3rem;
  height: 0.6rem;
  font-size: 0.3rem;
  border-radius: 0.12rem;
  border: 1px solid #ccc;
  color: #000;
}
.ui-reward-title {
  position: relative;
  /* height: 0.8rem; */
}
.ui-reward-title > p {
  margin: 0.16rem 0;
  text-align: center;
}
.ui-reward-title > a {
  display: block;
  font-size: 0.25rem;
  border: 1px solid #ccc;
  width: 3.6rem;
  padding: 0.08rem 0;
  text-align: center;
  border-radius: 0.14rem;
  position: absolute;
  right: 0;
}
</style>
