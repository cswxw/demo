// ==UserScript==
// @name         csdn hide adv and show
// @namespace    http://cswhello.com/
// @version      0.1
// @description  this is a test
// @author       cswhello
// @match        https://blog.csdn.net/*
// @match        https://bbs.csdn.net/topics/*
// @require      https://code.jquery.com/jquery-2.1.4.min.js
// @run-at       document-end
// @grant        none
// ==/UserScript==

(function() {
    'use strict';

    var currentURL = window.location.href;
    var bbs = /bbs\.csdn\.net/;
    var blog = /blog\.csdn\.net/;


    if(blog.test(currentURL)){
        $(".csdn-tracking-statistics.mb8.box-shadow").remove();
        $(".recommend-item-box.recommend-ad-box").remove();
        $("#dmp_ad_58").remove();

        $(".pulllog-box").hide();
        $("#adContent").hide();

        $("#_360_interactive").hide();
        $(".hide-article-box").hide();
        $("#article_content").height("100%");
        $($("#asideFooter").children("div").get(0)).hide();
        csdn.copyright.init("", "", "");
    }else if(bbs.test(currentURL)){
        $("#bbs_detail_wrap").css("max-height","");
        $("#bbs_detail_wrap").css("overflow","");
        $(".mediav_ad").remove();
        $("[class='bbs_feed bbs_feed_ad_box']").remove();

        $(".pulllog-box").hide();
        $("#adContent").hide();

        $(".hide_topic_box").remove();
    }



})();


