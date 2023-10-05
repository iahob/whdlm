<template>

</template>

<script>
import { gettokenc } from '../fetch/fetch'
export default {
  name: 'index',
  props: {
    token: String
  },
  mounted() {
   // 根据登录来源判断是否需要登录
      //localStorage.removeItem('token')
        gettokenc({ token:'' }, data => {
        if (data.data.valid) {
         this.token = data.data.token;
        }
        else{
            this.$router.push('/Login')
           return;
        }
      //console.log(data.data.valid);

      // console.log(this.token)
    if (this.token) {
    
      localStorage.setItem('token', this.token)
    }
     if(localStorage.getItem('token')) {
       this.$router.push('/Home')
       return
     }
       this.$router.push('/Login')
      })
  }
}
</script>
