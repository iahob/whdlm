<template>
  <div class="ui-login ui-main">
    <top></top>
    <div class="ui-container">
      <form>
        <div class="ui-panel">
          <div class="ui-form-item">
            <img title="代理账号" :src="imgUrl.phoneImg" />
            <input type="text" class="ui-value" placeholder="请输入您的代理账号" v-model="gameid">
          </div>
          <div class="ui-form-item">
            <img title="安全密码" :src="imgUrl.passImg" />
            <input type="password" class="ui-value" placeholder="请输入您的安全密码" v-model="password">
          </div>
          <div class="ui-form-item">
            <input type="submit" :disabled="disabled" value="登录" @click="login">
          </div>
        </div>
      </form>
    </div>
    <div class="ui-hints">
      <p>长按复制以下链接地址可以在微信中直接进入代理管理后台：
        <span>{{url}}</span>
      </p>
    </div>
    <dailog :show="showMessage">
      <message :msg="msg" v-on:close="closeDialog" :state="false"></message>
    </dailog>
  </div>
</template>
<script>
import top from './common/top'
import dailog from './common/dialog'
import message from "./common/message";
import { login } from '../fetch/fetch'
import md5 from 'blueimp-md5'

export default {
  name: 'login',
  components: { top, dailog, message },
  data: function() {
    return {
      imgUrl: {
        phoneImg: './assets/images/icon-9.png',
        passImg: './assets/images/icon-6.png'
      },
      gameid: null,
      password: null,
      showMessage: false,
      msg: null,
      disabled: false,
      url: ''
    }
  },
  created() {
    this.url = document.querySelector('#app').getAttribute('data-wcurl') || 'http://qi.foxuc.net/card/index.aspx';
    this.gameid=this.$route.query.gameid;
    console.log("111"+this.gameid);
  },
  methods: {
    cleanInput: function() {
      this.phone = null
      this.password = null
    },
    validate: function() {
      switch (true) {
        case !this.gameid:
          this.msg = '抱歉，代理账号不能为空'
          this.showMessage = true
          break
        case !this.password:
          this.msg = '抱歉，安全密码不能为空'
          this.showMessage = true
          break
        default:
          return false
      }
      this.cleanInput()
      return true
    },
    login: function(e) {
      e.preventDefault()

      if (this.validate()) {
        return
      }

      this.disabled = true

      let params = {
        gameid: this.gameid,
        pass: md5(this.password).toUpperCase()
      }
      login(params, data => {
        if (data.code === 401) {
          this.msg = '账号或密码错误，请重新输入，建议复制屏幕下方地址在微信中打开'
          this.showMessage = true
        } else if (data.data.valid === true) {
          localStorage.setItem('token', data.data.token)
          this.$router.push('/Home')
        }
        this.cleanInput()
        this.disabled = false
        // 登录失败处理还没有写
      })
    },
    closeDialog: function() {
      this.showMessage = false
    }
  }
}
</script>
<style scoped>
@import '../assets/css/inputStyle.css';

.ui-container {
  padding: 0 0.4rem;
}
.ui-form-item {
  text-align: center;
}
.ui-hints {
  position: fixed;
  width: 100%;
  bottom: 0;
  text-align: center;
  color: red;
}
.ui-panel {
  width: 100%;
  padding: 0.1rem 0 0 0;
  background: #f7f7f7;
  margin-top: 1rem;
}
.ui-form-item {
  margin-top: 0.4rem;
}
.ui-form-item:last-child {
  margin-top: 0.6rem;
}
.ui-form-item > img {
  width: 0.5rem;
  position: relative;
  top: 0.14rem;
}
.ui-login input[type='password'] {
  background: #f7f7f7;
  border-bottom: 1px solid #dedfe0;
}
.ui-login input[type='text'] {
  background: #f7f7f7;
  border-bottom: 1px solid #dedfe0;
}
@media screen and (min-width: 768px) {
  .ui-form-item {
    width: 70%;
    margin: 0 auto;
    margin-top: 0.3rem;
  }
}
</style>
