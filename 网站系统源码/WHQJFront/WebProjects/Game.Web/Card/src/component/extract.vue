<template>
  <div class="ui-dialog-message ui-message">
    <div class="ui-message-close">
      提取{{info.content}}
      <a @click="$emit('close')"><img :src="closeImg"/></a>
    </div>
    <div class="ui-message-content">
      <div class="ui-panel">
        <div class="ui-take">
          <p>可提{{info.content}} ：<span>{{info.award}}</span></p>
        </div>
        <div class="ui-form-item">
          <label>游戏&nbsp;I&nbsp;D&nbsp; ：</label>
          <input type="text" class="ui-value" v-model="gameId" @change="gameIdChange" placeholder="不填提取给自己">
        </div>
        <div class="ui-form-item">
          <label>游戏昵称 ：</label>
          <input type="text" class="ui-value ui-input-text" v-model="nickName" disabled placeholder="输入游戏ID验证昵称">
        </div>
        <div class="ui-form-item" v-show="isNeedPassword">
          <label>安全密码 ：</label>
          <input type="password" class="ui-value" v-model="password" />
        </div>
        <div class="ui-form-item">
          <label>提取{{info.content}} ：</label>
          <input type="text" class="ui-value" v-model="extract"/>
        </div>
      </div>
      <p style='text-align:center;' v-show="msg">{{msg}}</p>
      <p class v-show="!msg">注 ：提取{{info.content}}账号需要预留{{info.limit}}{{info.content}} ，不填ID默认提取给自己，提取到指定账号需要填写对应ID及安全密码</p>
    </div>
    <button class="ui-confirm" @click="giveAward">提&ensp;取</button>
  </div>
</template>
<script>
import { present, getNickNameByGameID } from "../fetch/fetch";
import md5 from "blueimp-md5";

export default {
  name: "extract",
  props: {
    info: Object,
    title: String,
    limit: {
      type: Number,
      default: 0
    },
    state: Boolean
  },
  data: function() {
    return {
      msg:null,
      gameId: null,
      nickName: null,
      nickExist: false,
      password: null,
      extract: null,
      isNeedPassword: false,
      closeImg: "/Card/v2/assets/images/close.png",
      failImg: "/Card/v2/assets/images/fail.png",
      successImg: "/Card/v2/assets/images/success.png"
    };
  },
  computed: {
  },
  updated:function (){
    if (this.info.cleanInput) {
      this.info.cleanInput = false;
      this.msg = null;
      this.cleanInput();
    }
  },
  methods: {
    cleanInput:function (){
      this.password = null,
      this.gameId = null,
      this.nickName = null,
      this.nickExist = false,
      this.isNeedPassword = false,
      this.extract = null
    },
    gameIdChange: function(e) {
      this.gameId = parseInt(e.target.value);
      // 通过gameID获取赠送昵称
      if (Number.isNaN(this.gameId)) {
        this.gameId = null;
        this.nickName = null;
        return;
      }
      getNickNameByGameID(
        { gameid: this.gameId, token: localStorage.getItem("token") },
        data => {
          const nick = data.data.NickName;
          if (nick) {
            this.nickName = nick;
            this.nickExist = true;
            this.isNeedPassword = this.gameId != this.info.GameID;
          } else {
            this.nickName = data.msg;
            this.nickExist = false;
          }
        }
      );
    },
    validate: function() {
      if (this.gameId && !this.nickExist) {
        this.msg = "提取的对象不存在";
        return true;
      }

      if (this.isNeedPassword && !this.password) {
        this.msg = "提取奖励给别人需要验证安全密码";
        return true;
      }
      if (!this.extract){
        this.msg = "请输入提取数量";
        return true;
      } 
      
      if (this.award < this.extract) {
        this.msg = "可提取数不足";
        return true;
      }
      return false;
    },
    giveAward: function(e) {
      e.preventDefault();

      if (this.validate()) {
        return;
      }

      this.disabled = true;
      let params = {
        token: localStorage.getItem("token"),
        gameid: this.gameId?this.gameId:0,
        count: this.extract,
        type: this.info.presentType,
        password: this.password?md5(this.password).toUpperCase():''
      };
      present(params, data => {
        this.showMessage = true;
        this.msg = data.msg;
        this.disabled = false;

        if (data.data.valid) {
          this.cleanInput();
          // 赠送成功后更新信息
          this.$emit("close");
          this.info.state = true;
        } else {
          this.info.state = false;
        }
      });
    }
  }
};
</script>
<style scoped>
.ui-message-content {
  text-align: center;
  width: 98%;
  margin: 0 auto;
}
.ui-message-content > img {
  margin-top: 0.4rem;
  width: 22%;
}
.ui-message-close {
  position: relative;
  left: 50%;
  top: 0.06rem;
  width: 98%;
  height: 0.6rem;
  background: #666;
  color: #fff;
  margin-left: -49%;
  border-radius: 0.1rem;
  text-align: center;
  font-size: 0.4rem;
}
.ui-message-close img {
  position: absolute;
  width: 7%;
  right: 0.1rem;
  top: 0.06rem;
}
.ui-confirm {
  background: #1aad19;
  border: none;
  color: #fff;
  font-size: 0.44rem;
  text-align: center;
  position: relative;
  left: 48%;
  top: -0.06rem;
  width: 98%;
  height: 0.8rem;
  margin-left: -47%;
  border-radius: 0.1rem;
}
.ui-dialog-message {
  position: absolute;
  z-index: 1001;
  left: 50%;
  top: 3rem;
  background: #fff;
  width: 7rem;
  margin-left: -3.5rem;
  border-radius: 0.14rem;
  box-shadow: 0rem 0rem 0.2rem rgb(85, 84, 84);
}
.ui-take > p {
  width: 96%;
  font-weight: 600;
  height: 0.5rem;
  text-align: left;
  padding: 0 0.1rem;
  margin: 0.26rem auto 0;
}
.ui-take > p > span {
  margin-left: 0.4rem;
}
.ui-message-content > p {
  font-size: 0.35rem;
  color: red;
  margin: 0.25rem auto 0.25rem;
  text-align: left;
  width: 96%;
  line-height: 1.5;
}
.ui-panel {
  background: #fff;
}
.ui-form-item > label {
  margin: 0.1rem auto 0;
  text-align: center;
  font-size: 0.36rem;
  font-weight: 600;
}
.ui-form-item {
  height: 0.8rem;
  border-bottom: 1px solid #dedfe0;
  width: 96%;
  margin: 0 auto 0.1rem;
  text-align: left;
  line-height: 1rem;
  display: flex;
  display: -webkit-flex;
}
.ui-form-item > input {
  margin-left: 0.4rem;
  margin-top: 0.2rem;
  flex: 1;
  -ms-flex: 1;
  font-weight: 600;
  font-size: 0.36rem;
}
@media screen and (max-width: 320px) {
  .ui-dialog-message {
    width: 6.2rem;
    margin-left: -3.1rem;
  }
}
</style>