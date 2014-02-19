window.gw2emblem || (gw2emblem = {});
gw2emblem.flags_defs = {
    FlipBackgroundVertical: 1,
    FlipBackgroundHorizontal: 2,
    FlipForegroundVertical: 4,
    FlipForegroundHorizontal: 8
};
gw2emblem.init = function (a, c, b) {
    gw2emblem.paper = Raphael(a, c, c);
    gw2emblem.pto2_color = "#000000";
    gw2emblem.pto2_op = 0.3;
    gw2emblem.pt1_op = 0.3;
    gw2emblem.bg_op = 0.3;
    gw2emblem.bg_color = b || "";
    gw2emblem.bg_img = "img_bg.png";
    gw2emblem.base_size = 256;
    gw2emblem.flip = 0;
    return gw2emblem
};
gw2emblem.drawEmblemGw2 = function (a) {
    gw2emblem.setFlipsGW2(a.flags);
    gw2emblem.drawEmblem(gw2emblem.defs[a.foreground_id] || "", gw2emblem.color_defs[a.foreground_secondary_color_id] || "#FFFFFF", gw2emblem.color_defs[a.foreground_primary_color_id] || "#FF0000", gw2emblem.bg_defs[a.background_id] || "", gw2emblem.color_defs[a.background_color_id] || "#000000")
};
gw2emblem.drawEmblem = function (a, c, b, e, d) {
    var g = gw2emblem.paper;
    g.clear();
    "" !== gw2emblem.bg_color ? g.rect(0, 0, g.width, g.height).attr({
        fill: gw2emblem.bg_color,
        stroke: gw2emblem.bg_color
    }) : g.image(gw2emblem.bg_img, 0, 0, g.width, g.height);
    gw2emblem.drawEmblemBg(e, d);
    gw2emblem.drawEmblemFg(a, c, b)
};
gw2emblem.drawEmblemFg = function (a, c, b) {
    var e = gw2emblem.paper,
        d;
    d = e.width / gw2emblem.base_size;
    var g = 1 != d ? "s".concat(d, ",", d, ",0,0") : "";
    3 < gw2emblem.flip && (g = g.concat(" s", 0 !== (gw2emblem.flip & 8) ? -1 : 1, ",", 0 !== (gw2emblem.flip & 4) ? -1 : 1, ",", gw2emblem.base_size / 2, ",", gw2emblem.base_size / 2));
    gw2emblem.paths = [];
    var h = gw2emblem.paths;
    if (a.p1)
        for (d = 0; d < a.p1.length; d++) h[h.length] = e.path(a.p1[d]).attr({
            fill: c,
            stroke: "none"
        }).transform(g);
    if (a.p2)
        for (d = 0; d < a.p2.length; d++) h[h.length] = e.path(a.p2[d]).attr({
            fill: b,
            stroke: "none"
        }).transform(g);
    if (a.pto2)
        for (d = 0; d < a.pto2.length; d++) h[h.length] = e.path(a.pto2[d]).attr({
            fill: gw2emblem.pto2_color,
            stroke: "none",
            opacity: gw2emblem.pto2_op
        }).transform(g);
    if (a.pt1)
        for (d = 0; d < a.pt1.length; d++) h[h.length] = e.path(a.pt1[d]).attr({
            fill: c,
            stroke: "none",
            opacity: gw2emblem.pt1_op
        }).transform(g);
    return h
};
gw2emblem.drawEmblemBg = function (a, c) {
    var b = gw2emblem.paper,
        e;
    e = b.width / gw2emblem.base_size;
    var d = 1 != e ? "s".concat(e, ",", e, ",0,0") : "";
    if (0 !== (gw2emblem.flip & 1) || 0 !== (gw2emblem.flip & 2)) d = d.concat(" s", 0 !== (gw2emblem.flip & 2) ? -1 : 1, ",", 0 !== (gw2emblem.flip & 1) ? -1 : 1, ",", gw2emblem.base_size / 2, ",", gw2emblem.base_size / 2);
    gw2emblem.bg_paths = [];
    var g = gw2emblem.bg_paths;
    if (a.p)
        for (e = 0; e < a.p.length; e++) g[g.length] = b.path(a.p[e]).attr({
            fill: c,
            stroke: "none",
            opacity: 0.75
        }).transform(d);
    return g
};
gw2emblem.setFlipsGW2 = function (a) {
    for (var c = gw2emblem.flip = 0; c < a.length; c++) gw2emblem.flags_defs[a[c]] && (gw2emblem.flip += gw2emblem.flags_defs[a[c]])
};
gw2emblem.color_defs = {
    1: "#7c6c53",
    2: "#252326",
    3: "#5f5c5c",
    4: "#484546",
    5: "#302e31",
    6: "#d3d0cf",
    7: "#003349",
    8: "#016a87",
    9: "#004c6d",
    10: "#3682a0",
    11: "#001f34",
    12: "#48220f",
    13: "#41311d",
    14: "#58402a",
    15: "#27251e",
    16: "#2a1607",
    17: "#472c17",
    18: "#361e03",
    19: "#353228",
    20: "#968469",
    21: "#301308",
    22: "#564a38",
    23: "#7e6343",
    24: "#653b22",
    25: "#221b0b",
    26: "#5f4320",
    27: "#24417a",
    28: "#4a71bb",
    29: "#04143e",
    30: "#122559",
    31: "#23578b",
    32: "#774b43",
    33: "#864135",
    34: "#5d2319",
    35: "#9f6a57",
    36: "#965040",
    37: "#596680",
    38: "#2e4153",
    39: "#203a44",
    40: "#2f3148",
    41: "#314a48",
    42: "#485f5d",
    43: "#28323f",
    44: "#2e525f",
    45: "#283a3d",
    46: "#3d5267",
    47: "#461041",
    48: "#5e1d64",
    49: "#793581",
    50: "#9d5ca4",
    51: "#2d0923",
    52: "#666000",
    53: "#586e39",
    54: "#8a8127",
    55: "#1d653b",
    56: "#00260e",
    57: "#0e2c0b",
    58: "#42865a",
    59: "#0a4f27",
    60: "#0f2f24",
    61: "#07351a",
    62: "#453b2e",
    63: "#3e4130",
    64: "#714910",
    65: "#3f3921",
    66: "#5d2e0f",
    67: "#8a6732",
    68: "#3e4447",
    69: "#21292d",
    70: "#a7998e",
    71: "#370400",
    72: "#464637",
    73: "#491a05",
    74: "#828a92",
    75: "#57534c",
    76: "#0e1c25",
    77: "#171b28",
    78: "#241620",
    79: "#261702",
    80: "#0f1c14",
    81: "#201906",
    82: "#1e1725",
    83: "#250f0b",
    84: "#2b1618",
    85: "#241209",
    86: "#0a1b1a",
    87: "#1a1829",
    88: "#161c0e",
    89: "#323c41",
    90: "#9a8372",
    91: "#7c6c53",
    92: "#403d46",
    93: "#7c888a",
    94: "#9b8e79",
    96: "#4b3f39",
    97: "#373f38",
    98: "#746970",
    99: "#5f6460",
    100: "#45463e",
    101: "#666455",
    102: "#ba6f57",
    103: "#615449",
    104: "#3c3900",
    105: "#455100",
    106: "#5b6a00",
    107: "#272400",
    108: "#747f21",
    109: "#a04b17",
    110: "#3b0c00",
    111: "#ca6b39",
    112: "#983f17",
    113: "#5f1700",
    114: "#452863",
    115: "#230f3d",
    116: "#5e468c",
    117: "#301a4d",
    118: "#8160af",
    119: "#330002",
    120: "#a9484c",
    121: "#c2616a",
    122: "#470000",
    123: "#660006",
    124: "#b65a80",
    125: "#751943",
    126: "#a9365e",
    127: "#4d0026",
    128: "#2f0019",
    129: "#405612",
    130: "#192700",
    131: "#163900",
    132: "#235000",
    133: "#62893c",
    134: "#2d8c7f",
    135: "#00514c",
    136: "#003831",
    137: "#006c6c",
    138: "#002323",
    139: "#353574",
    140: "#292159",
    141: "#6b6eb9",
    142: "#4d5398",
    143: "#151340",
    144: "#7a4400",
    145: "#5e3100",
    146: "#895500",
    147: "#9b6c00",
    148: "#ab8726",
    314: "#483f42",
    315: "#af9f7c",
    332: "#9c6a46",
    333: "#927743",
    334: "#827b3e",
    335: "#86706b",
    336: "#998b70",
    337: "#765f54",
    338: "#562400",
    339: "#426d23",
    340: "#514500",
    341: "#453000",
    342: "#2c1d00",
    343: "#b96b6b",
    344: "#bd8861",
    345: "#bb9755",
    346: "#959b5f",
    347: "#7d9664",
    348: "#68947b",
    349: "#698f97",
    350: "#7579a1",
    351: "#8a6c99",
    352: "#945648",
    353: "#736751",
    354: "#a37973",
    355: "#8f8f71",
    356: "#738083",
    357: "#8d7277",
    358: "#2b322e",
    359: "#6d7a4a",
    360: "#5e785b",
    361: "#50706a",
    362: "#506773",
    363: "#5b5e78",
    364: "#715063",
    365: "#814646",
    366: "#765632",
    367: "#4b4422",
    368: "#57291b",
    369: "#5a3808",
    370: "#273300",
    371: "#4a3751",
    372: "#302a41",
    373: "#7f7363",
    374: "#c55d4b",
    375: "#9d3e2d",
    376: "#85241a",
    377: "#580800",
    378: "#470000",
    379: "#b4752a",
    380: "#98560b",
    381: "#864000",
    382: "#7b3000",
    383: "#624225",
    384: "#4b382e",
    385: "#5b5f63",
    434: "#6f4326",
    435: "#5a4620",
    436: "#4d2121",
    437: "#552f16",
    438: "#383c16",
    439: "#514a64",
    440: "#36273f",
    441: "#482a3d",
    442: "#8e8a78",
    443: "#bdbab9",
    444: "#857a60",
    445: "#8e9881",
    446: "#7f7272",
    447: "#a0826c",
    448: "#7d8674",
    449: "#777987",
    450: "#4b3a2d",
    451: "#2c3439",
    452: "#6b5b3c",
    453: "#392920",
    454: "#473f33",
    455: "#2f281c",
    456: "#21130e",
    457: "#4d3e26",
    458: "#292c36",
    459: "#4f3929",
    460: "#807383",
    461: "#74867d",
    462: "#9e8b66",
    463: "#4a2a2a",
    464: "#342d38",
    465: "#393522",
    466: "#6c3e2d",
    467: "#695637",
    468: "#88533f",
    469: "#483b1b",
    470: "#2b230c",
    471: "#3d2b31",
    472: "#4a361a",
    473: "#1a181b",
    474: "#524f4f",
    475: "#6b6969",
    476: "#3b393c",
    477: "#9d9a9a",
    478: "#624c3f",
    479: "#81654d",
    480: "#9c7e51",
    481: "#a16d55",
    482: "#000000",
    483: "#000008",
    484: "#323b2d",
    485: "#343223",
    582: "#752200",
    583: "#21356a",
    584: "#9d8e6c",
    585: "#c7a23d",
    586: "#330000",
    587: "#a0a8b9",
    588: "#284e82",
    589: "#523776",
    590: "#6687c6",
    591: "#e09e5c",
    592: "#770000",
    593: "#6b1400",
    594: "#e38350",
    595: "#ee9566",
    596: "#d8aa86",
    597: "#414f53",
    598: "#b2a740",
    599: "#1a4400",
    600: "#a97815",
    601: "#371700",
    602: "#322f00",
    603: "#595fa9",
    604: "#411e00",
    605: "#382800",
    606: "#00453e",
    607: "#103300",
    608: "#500c00",
    609: "#3a0f32",
    610: "#7d7a7a",
    611: "#8e8a88",
    612: "#687600",
    613: "#7cb286",
    614: "#7895cd",
    615: "#662d6c",
    616: "#893203",
    617: "#1c5a2d",
    618: "#4d2a00",
    619: "#b34d74",
    620: "#a366aa",
    621: "#d8b080",
    622: "#ac6620",
    623: "#cc8a48",
    624: "#b7678b",
    625: "#005f7c",
    626: "#42447f",
    627: "#47514b",
    628: "#ccb471",
    629: "#5a0000",
    630: "#63a46f",
    631: "#9cc4a0",
    632: "#2a5c05",
    633: "#b14d3b",
    634: "#22788f",
    635: "#75974a",
    636: "#86a956",
    637: "#a9c188",
    638: "#499864",
    639: "#43978c",
    640: "#6369b5",
    641: "#8e2652",
    642: "#d4b254",
    643: "#888266",
    644: "#382f67",
    645: "#6b654c",
    646: "#b5bc72",
    647: "#b95629",
    648: "#97322c",
    649: "#8fa7d3",
    650: "#e7a876",
    651: "#e0ae73",
    652: "#dabc6b",
    653: "#8ac094",
    654: "#9ebc75",
    655: "#b3bc5e",
    656: "#efab8b",
    657: "#f7aa9d",
    658: "#a895c3",
    659: "#cb8ca9",
    660: "#83bbac",
    661: "#c1b759",
    662: "#9398d6",
    663: "#b88dbc",
    664: "#85b3c1",
    665: "#d9ab95",
    666: "#a94165",
    667: "#884a8e",
    668: "#d8aca4",
    669: "#e18076",
    670: "#541c54",
    671: "#9b83bc",
    672: "#b1a4c3",
    673: "#87000a",
    674: "#00405b",
    675: "#3c2456",
    676: "#386299",
    677: "#6e3b00",
    678: "#bd7294",
    679: "#c99eb2",
    680: "#610a35",
    681: "#6bac9e",
    682: "#55ad9a",
    683: "#96bab1",
    684: "#787dc0",
    685: "#8f77b0",
    686: "#642700",
    687: "#736d00",
    688: "#5b4e00",
    689: "#297549",
    690: "#97a53b",
    691: "#488f9f",
    692: "#496100",
    693: "#c5bd70",
    694: "#4b772a",
    695: "#7b7600",
    696: "#01776e",
    697: "#9d922a",
    698: "#a0252f",
    699: "#f39a91",
    700: "#3d70a2",
    701: "#765c9d",
    702: "#f79b7e",
    703: "#a99c81",
    704: "#005d5d",
    705: "#431a00",
    706: "#6b5490",
    707: "#868bc9",
    708: "#9da0c4",
    709: "#064224",
    710: "#ac7db1",
    711: "#bc9dbf",
    712: "#6fa2b1",
    713: "#5699ae",
    714: "#96b1ba",
    715: "#89932a",
    1053: "#e17967",
    1054: "#3e4100",
    1149: "#791a09",
    1150: "#440000",
    1151: "#601300",
    1152: "#2d0000",
    1153: "#550000",
    1154: "#a1660c",
    1155: "#c58d36",
    1156: "#455139",
    1157: "#3e574f",
    1158: "#42485a",
    1159: "#47435a",
    1160: "#573e4f",
    1161: "#574747",
    1231: "#3c6a65",
    1232: "#3d5e6f",
    1233: "#597d8f",
    1234: "#578984",
    1235: "#2f4d4a",
    1236: "#304550",
    1237: "#cb8c16",
    1238: "#dab44d",
    1239: "#9c5915",
    1240: "#8b2e12",
    1241: "#423a31",
    1242: "#8e8b73"
};
jQuery(document).ready(initmap);
jQuery(window).resize(windowresize);
var version = 8,
    map, map_active = !0,
    zones = [],
    sectors = [],
    icon_waypoint, icon_marker, icon_dungeon, movingstep = 0,
    tiles, pos, poszoom = 7,
    layer_wp, layer_poi, layer_vista, layer_skillpoint, layer_heart, layer_jp, layer_explorator, layer_dungeon, layer_delpath, layer_prequests, layer_events, layer_nodes, allowclick = !0,
    mb_timer, movetimer, loaded_marker = [],
    loaded_sector = [],
    menuopen = !1,
    multiopen = !1,
    multinameopen = !1,
    rights = [],
    deletemarker, searchformarker, geosettings = [],
    showmarker = L.marker(),
    mapload = 1,
    categories = [],
    catelements = [],
    selected_tool = 0,
    paths = [],
    path_layer = [],
    specialmarker = 0,
    events = [],
    event_timer, embedded = !1,
    serverdataloaded = !1,
    loadevents = !1,
    loaded_event_details = !1,
    loaded_map_details = !1,
    forced_login = !1,
    angemeldet = !1,
    delay_parameter = "",
    gad_visible = !1,
    gad_timer_active = !1,
    mapmode = 1,
    zonelist = [],
    sectorlist = [],
    oldserver = 0,
    loaded_match = !1,
    match_id, red, blue, green, old_owner = [],
    wvwstatusopen = !1,
    countdownlist = [],
    guildcache = [],
    taglist = [],
    sinuseinhundert = [],
    gutts = 1,
    special_active = !1,
    special_events = Array({
        dom_id: "event-ableger",
        icon: L.icon({
            iconUrl: "images/toxicroot.png",
            iconSize: [32, 32],
            iconAnchor: [15, 16]
        }),
        keywords: ["ableger"],
        event_ids: []
    });

function togglemenu() {
    var a = jQuery(window).innerWidth() - 0,
        c = 1E3;
    menuopen || (a -= 400, c = 300);
    menuopen = !menuopen;
    jQuery("#menu").animate({
        left: a
    }, c)
}

function togglegad() {
    gad_timer_active && window.clearTimeout(gad_delay);
    gad_timer_active = !1;
    var a;
    gad_visible ? (gad_visible = !1, a = jQuery(window).innerHeight() + 10, "undefined" != typeof gad_delay2 && (window.clearTimeout(gad_delay2), gad_delay2 = void 0)) : (gad_visible = !0, a = jQuery(window).innerHeight() - 70, jQuery("#gad").css({
        width: 486
    }));
    jQuery("#gad").animate({
        top: a
    }, 2E3)
}
var gad_timer_active = !0,
    gad_delay = window.setTimeout(togglegad, 9E4);

function windowresize() {
    if (!embedded) {
        var a = (jQuery(window).innerHeight() - 700) / 2,
            c = jQuery(window).innerWidth() - 0;
        menuopen && (c -= 400);
        jQuery("#menu").offset({
            top: a,
            left: c
        });
        c = (jQuery(window).innerWidth() - jQuery("#multi-editor").width()) / 2;
        jQuery("#multi-editor").offset({
            left: c
        });
        c = (jQuery(window).innerWidth() - jQuery("#multi-name").width()) / 2;
        jQuery("#multi-name").offset({
            left: c
        });
        a = gad_visible ? jQuery(window).innerHeight() - 70 : jQuery(window).innerHeight() + 10;
        c = (jQuery(window).innerWidth() - 478) / 2;
        jQuery("#gad").offset({
            top: a,
            left: c
        });
        a = jQuery(window).innerHeight() - 80 - 80 - 230 - 10;
        jQuery("#wvwlog").css({
            height: a
        });
        jQuery("#wvwexpander").css({
            height: a
        })
    }
}

function tryupdate() {
    jQuery("#updatebutton").hide();
    menuopen && togglemenu();
    notify_user("Dieser Vorgang kann eine Weile dauern, bitte warten...");
    deactivate_map();
    jQuery.post("ajax.php", {
        action: "update"
    }, function (a) {
        if (a = eval("(" + a + ")")) alert(a.result), jQuery("#updatebutton").fadeIn(), activate_map()
    })
}

function dologout() {
    jQuery("#username").focus();
    rights = [];
    jQuery("#abmeldemaske").slideUp();
    jQuery("#anmeldemaske").slideDown();
    notify_user("Auf Wiedersehen!");
    jQuery.post("ajax.php", {
        action: "logout"
    }, function (a) {
        (a = eval("(" + a + ")")) && "ok" == a.result && (jQuery.post("ajax.php", {
            action: "logout"
        }), forced_login && update_categories())
    });
    logged_in(!1)
}

function dologin() {
    var a = jQuery("#username").val(),
        c = jQuery("#passwort").val();
    jQuery("#passwort").val("");
    jQuery.post("ajax.php", {
        action: "login",
        username: a,
        password: c
    }, function (a) {
        if (a = eval("(" + a + ")")) notify_user(a.message), "ok" == a.result ? (jQuery("#username").val(""), togglemenu(), rights = a.rights, userid = a.userid, user_source = a.source, jQuery("#abmeldemaske").slideDown(), jQuery("#anmeldemaske").slideUp(), logged_in(!0)) : (jQuery("#username").focus(), rights = [], jQuery("#abmeldemaske").slideUp(), jQuery("#anmeldemaske").slideDown(),
            logged_in(!1))
    })
}

function checklogin() {
    jQuery.post("ajax.php", {
        action: "checklogin"
    }, function (a) {
        if (a = eval("(" + a + ")")) "ok" == a.result ? (rights = a.rights, userid = a.userid, user_source = a.source, notify_user("Angemeldet als " + a.username), jQuery("#abmeldemaske").slideDown(), jQuery("#anmeldemaske").slideUp(), logged_in(!0)) : (rights = [], jQuery("#abmeldemaske").slideUp(), jQuery("#anmeldemaske").slideDown(), logged_in(!1)), parse_startparameter()
    })
}

function logged_in(a) {
    (angemeldet = a) ? (gad_timer_active && (window.clearTimeout(gad_delay), gad_timer_active = !1), gad_visible ? 1 == rights.hideads && togglegad() : 1 != rights.hideads && togglegad(), 2 != mapmode && (jQuery("#layer-event").show().prev("h3").show(), jQuery("#layer-search").show().prev("h3").show()), jQuery("#layer-parameter").show().prev("h3").show(), jQuery("#praecat").accordion("refresh"), 1 == rights.categories && (jQuery("#praekat").after('<h3>Kategorienverwaltung</h3><div id="katman"></div>'), jQuery("#menu-accordion").accordion("refresh")),
        update_categories(), 1 == rights.update && jQuery("#updatebutton").fadeIn(), 0 < delay_parameter.length && (parse_startparameter(delay_parameter), delete delay_parameter)) : (rights = [], user_source = userid = void 0, gad_visible || gad_timer_active || togglegad(), jQuery("#updatebutton").hide(), jQuery("#katman").prev().remove(), jQuery("#katman").remove(), load_categories(), forced_login && (jQuery("#layer-event").hide().prev("h3").hide(), jQuery("#layer-search").hide().prev("h3").hide(), jQuery("#layer-parameter").hide().prev("h3").hide(),
        jQuery("#praecat").accordion("refresh").accordion("option", "active", 0)))
}

function change_password() {
    if ("phpbb" == user_source) jQuery("#dialog").html("<p>Du bist \u00fcber Deinen Foren-Account angemeldet und musst daher das Passwort im Forum \u00e4ndern. Mit einem Klick auf \u00bbweiter\u00ab gelangst Du direkt dorthin.</p>").dialog({
        title: "Passwort \u00e4ndern",
        resizable: !1,
        modal: !0,
        width: 500,
        buttons: [{
            text: "ne, dann hab' ich jetzt doch keine Lust",
            click: function () {
                jQuery(this).dialog("close")
            }
        }, {
            text: "weiter",
            click: function () {
                window.open("http://forum.wyld-bunch.de/ucp.php?i=profile&mode=reg_details");
                dologout();
                jQuery(this).dialog("close")
            }
        }],
        show: {
            effect: "drop",
            duraction: 1E3
        },
        hide: {
            effect: "explode",
            duration: 1E3
        }
    });
    else {
        var a;
        a = '<p>Achte darauf, dass das Passwort aus mindestens 8 Zeichen besteht und Gro\u00df-, Kleinbuchstaben und Ziffern enth\u00e4lt.</p><div id="errormessage" style="display: none;background-color:#fcc"></div><fieldset><label for="jetzt">aktuelles Passwort:</label><input type="password" class="text" id="jetzt" /><br /><label for="neu1">neues Passwort:</label><input type="password" class="text" id="neu1" /><label for="neu2">erneut neues Passwort:</label><input type="password" class="text" id="neu2" /></fieldset>';
        jQuery("#dialog").html(a).dialog({
            title: "Passwort \u00e4ndern",
            resizable: !1,
            modal: !0,
            width: 500,
            buttons: [{
                text: "ne, ach, das jetzige ist doch ganz nett",
                click: function () {
                    jQuery(this).dialog("close")
                }
            }, {
                text: "\u00e4ndern",
                click: function () {
                    jQuery("#errormessage").hide();
                    var a = !0,
                        b = "",
                        e = jQuery("#jetzt").val(),
                        d = jQuery("#neu1").val(),
                        g = jQuery("#neu2").val();
                    if (d != g) a = !1, b += "<p>Die Passw\u00f6rter stimmen nicht \u00fcberein.</p>";
                    else {
                        8 > d.length && (a = !1, b += "<p>Das Passwort ist zu kurz</p>");
                        var g = /[a-z]+/,
                            h = /\d+/;
                        /[A-Z]+/.test(d) && g.test(d) && h.test(d) || (a = !1, b += "<p>Das Passwort enth\u00e4lt nicht, wie gefordert, Gro\u00df-, Kleinbuchstaben und Ziffern.</p>")
                    }
                    a ? jQuery.post("ajax.php", {
                        action: "changepassword",
                        alt: e,
                        neu: d
                    }, function (a) {
                        a = eval("(" + a + ")");
                        1 == a.result ? (jQuery("#dialog").dialog("close"), notify_user("Passwort erfolgreich ge\u00e4ndert. Bitte erneut anmelden."), dologout()) : jQuery("#errormessage").html(a.result).show()
                    }) : jQuery("#errormessage").html(b).show()
                }
            }],
            show: {
                effect: "drop",
                duraction: 1E3
            },
            hide: {
                effect: "explode",
                duration: 1E3
            }
        })
    }
}

function get_icon(a) {
    return 1 == a ? "images/waypoint32.png" : 2 == a ? "images/poi.png" : 3 == a ? "images/vista.png" : 4 == a ? "images/skillpoint.png" : 5 == a ? "images/heart.png" : 6 == a ? "images/jp.png" : 7 == a ? "images/dungeon.png" : 8 == a ? "images/explorator-32.png" : 12 == a ? "images/little-flag.png" : 11 == a ? "images/cursor.png" : 13 == a ? "images/sector.png" : "images/little-flag.png"
}

function clear_all_layers() {
    "undefined" != typeof layer_delpath && layer_delpath.clearLayers();
    "undefined" != typeof aniPaths && aniPaths.clearLayers();
    "undefined" != typeof aniMarker && aniMarker.clearLayers();
    "undefined" != typeof layer_prequests && layer_prequests.clearLayers();
    "undefined" != typeof layer_events && layer_events.clearLayers();
    "undefined" != typeof layer_nodes && layer_nodes.clearLayers();
    map.removeLayer(showmarker)
}

function update_categories() {
    jQuery("#katman").html("updating...");
    jQuery.post("ajax.php", {
        action: "getcategories"
    }, function (a) {
        if (a = eval("(" + a + ")")) {
            categories = a.categories;
            catelements = a.catelements;
            var c;
            c = '<p><input id="add_cat" type="text" /><input type="button" id="add_cat_button" value="Kategorie hinzuf\u00fcgen" /></p><hr />';
            c += '<p><label for="cat1">Kategorie: </label>';
            c += '<select id="cat1">';
            if (a.categories)
                for (var b = 0; b < a.categories.length; b++) 0 == a.categories[b].parent && (c += '<option value="' +
                    a.categories[b].id + '">' + a.categories[b].name + "</option>");
            c += "</select>";
            c += '<input id="del_cat_button" type="button" value="l\u00f6schen" /></p>';
            c += '<p><input id="add_subcat" type="text" /><input id="add_subcat_button" type="button" value="Unterkategorie hinzuf\u00fcgen" /></p>';
            c += "<hr />";
            c += '<p><label for="cat2">Unterkategorie: </label>';
            c += '<select id="cat2">';
            c += "</select>";
            c += '<input id="del_subcat_button" type="button" value="l\u00f6schen" /></p>';
            jQuery("#katman").html(c);
            jQuery("#add_cat_button").click(add_category);
            jQuery("#del_cat_button").click(del_category);
            jQuery("#add_subcat_button").click(add_subcategory);
            jQuery("#del_subcat_button").click(del_subcategory);
            jQuery("#cat1").change(refresh_sub_man_menu);
            update_cat_menu();
            refresh_sub_man_menu();
            window.setTimeout(function () {
                jQuery("#menu-accordion").accordion("refresh")
            }, 5E3)
        }
    })
}

function refresh_sub_man_menu() {
    for (var a = jQuery("#cat1").val(), c = "", b = 0; b < categories.length; b++) categories[b].parent == a && (c += '<option value="' + categories[b].id + '">' + categories[b].name + "</option>");
    jQuery("#cat2").html(c)
}

function del_subcategory() {
    jQuery("#del_subcat_button").fadeOut();
    var a = jQuery("#cat2").val();
    jQuery.post("ajax.php", {
        action: "delsubcategory",
        id: a
    }, function (a) {
        if (a = eval("(" + a + ")")) notify_user(a.result), update_categories()
    })
}

function deactivate_map() {
    map_active = !1;
    map.dragging.disable();
    map.touchZoom.disable();
    map.scrollWheelZoom.disable();
    map.doubleClickZoom.disable();
    map.boxZoom.disable();
    map.keyboard.disable();
    map.closePopup();
    jQuery(".leaflet-control-zoom").fadeOut();
    jQuery("#animation").fadeIn()
}

function activate_map() {
    map_active = !0;
    map.dragging.enable();
    map.touchZoom.enable();
    map.scrollWheelZoom.enable();
    map.doubleClickZoom.enable();
    map.boxZoom.enable();
    map.keyboard.enable();
    jQuery(".leaflet-control-zoom").fadeIn();
    jQuery("#animation").fadeOut()
}

function add_subcategory() {
    jQuery("#add_subcat_button").fadeOut();
    var a = jQuery("#add_subcat").val(),
        c = jQuery("#cat1").val();
    jQuery.post("ajax.php", {
        action: "addsubcategory",
        name: a,
        parent: c
    }, function (a) {
        if (a = eval("(" + a + ")")) notify_user(a.result), update_categories()
    })
}

function del_category() {
    jQuery("#del_cat_button").fadeOut();
    var a = jQuery("#cat1").val();
    jQuery.post("ajax.php", {
        action: "delcategory",
        id: a
    }, function (a) {
        if (a = eval("(" + a + ")")) notify_user(a.result), update_categories()
    })
}

function add_category() {
    jQuery("#add_cat_button").fadeOut();
    var a = jQuery("#add_cat").val();
    jQuery.post("ajax.php", {
        action: "addcategory",
        name: a
    }, function (a) {
        if (a = eval("(" + a + ")")) notify_user(a.result), update_categories()
    })
}

function load_categories() {
    jQuery.post("ajax.php", {
        action: "getcategories"
    }, function (a) {
        if (a = eval("(" + a + ")")) categories = a.categories, catelements = a.catelements, update_cat_menu()
    })
}

function update_cat_menu() {
    jQuery(".category").prev().remove();
    jQuery(".category").remove();
    var a = "";
    "undefined" == typeof categories && (categories = []);
    for (var c = 0; c < categories.length; c++)
        if (0 == categories[c].parent) {
            for (var a = a + ("<h3>" + categories[c].name + "</h3>"), a = a + ('<div class="category" id="category' + categories[c].id + '">'), b = "", e = 0; e < catelements.length; e++)
                if (catelements[e].cat1 == categories[c].id && 0 == catelements[e].cat2) {
                    var d = "";
                    1 == rights.multi && (d = ' <a href="javascript:edit_multi(' + catelements[e].id +
                        ');"><img src="images/edit-16.png" /></a>');
                    b += '<li><a href="' + window.location.protocol + "//" + window.location.host + window.location.pathname + "?special=" + catelements[e].id + '" onclick="gotospecial(' + catelements[e].id + ');return false;">' + catelements[e].name + "</a>" + d + "</li>"
                }
            5 < b.length && (a += "<ul>" + b + "</ul>");
            a += "</div>"
        }
    jQuery("#praecat").after(a);
    2 == mapmode && (jQuery(".category").hide().prev().hide(), jQuery("#praecat").accordion("refresh").accordion("option", "active", 0));
    jQuery("#menu-accordion").accordion("refresh");
    for (c = 0; c < categories.length; c++)
        if (0 != categories[c].parent) {
            a = "";
            a += "<h3>" + categories[c].name + "</h3>";
            a += '<div class="category" id="category' + categories[c].id + '">';
            b = "";
            for (e = 0; e < catelements.length; e++) catelements[e].cat1 == categories[c].parent && catelements[e].cat2 == categories[c].id && (d = "", 1 == rights.multi && (d = ' <a href="javascript:edit_multi(' + catelements[e].id + ');"><img src="images/edit-16.png" /></a>'), b += '<li><a href="' + window.location.protocol + "//" + window.location.host + window.location.pathname +
                "?special=" + catelements[e].id + '" onclick="gotospecial(' + catelements[e].id + ');return false;">' + catelements[e].name + "</a>" + d + "</li>");
            5 < b.length && (a += "<ul>" + b + "</ul>");
            a += "</div>";
            jQuery("#category" + categories[c].parent).append(a);
            jQuery("#category" + categories[c].parent).accordion({
                heightStyle: "content",
                collapsible: !0,
                active: !1
            });
            jQuery("#category" + categories[c].parent).accordion("refresh")
        }
        "undefined" != typeof autoopen && (menuopen || togglemenu(), jQuery("#menu-accordion>h3").each(function (a, c) {
        if ("undefined" !=
            typeof autoopen) {
            var b = jQuery(c).html(),
                d = eval("/" + autoopen.replace(/\//g, "\\/") + "/i");
            "anmelden" == autoopen && window.setTimeout(function () {
                jQuery("#username").focus()
            }, 1600);
            b.match(d) && (window.setTimeout(function () {
                jQuery("#menu-accordion").accordion("option", "active", a)
            }, 1500), delete autoopen)
        }
        "undefined" != typeof autoopen && jQuery(c).next("div").children("h3").each(function (c, b) {
            if ("undefined" != typeof autoopen) {
                var d = jQuery(b).html(),
                    e = eval("/" + autoopen.replace(/\//g, "\\/") + "/i");
                d.match(e) && (window.setTimeout(function () {
                    jQuery("#menu-accordion").accordion("option",
                        "active", a)
                }, 1500), window.setTimeout(function () {
                    jQuery(b).parent().accordion("option", "active", c)
                }, 3E3), delete autoopen)
            }
        })
    }), "undefined" != typeof autosuche && window.setTimeout(function () {
        jQuery("#searchfield").val(autosuche);
        start_search()
    }, 4500))
}

function initmap() {
    for (var a = window.location.search.substr(1).split("&"), c = 0; c < a.length; c++) {
        var b = a[c].split("=");
        "mode" == b[0] && "embedded" == b[1] && (embedded = !0)
    }
    jQuery("#gadclosebutton").click(function () {
        !0 == gad_visible && togglegad()
    });
    jQuery("#noapi").hide();
    for (c = b = a = 0; 100 >= c; c++) a += Math.sin(Math.PI / 100 * c);
    for (c = 0; 100 >= c; c++) b += Math.sin(Math.PI / 100 * c), sinuseinhundert[c] = b / a;
    b = a = void 0;
    c = get_cookie("messagetime");
    "" == c && (c = 2);
    jQuery("#messagetimetext").text(c + " Minute" + (1 < c ? "n" : ""));
    jQuery("#messagetime").slider({
        animate: "slow",
        max: 10,
        min: 1,
        range: "min",
        step: 1,
        value: c,
        change: function (a, c) {
            var b = jQuery(this).slider("value");
            jQuery("#messagetimetext").text(b + " Minute" + (1 < b ? "n" : ""));
            set_cookie("messagetime", b)
        }
    });
    c = get_cookie("timestamps");
    "" == c && (c = 0);
    1 == c && jQuery("#timestamp").attr("checked", !0);
    jQuery("#timestamp").change(function () {
        jQuery(this).prop("checked") ? set_cookie("timestamps", 1) : set_cookie("timestamps", 0)
    });
    c = get_cookie("ruins");
    "" == c && (c = 0);
    1 == c && jQuery("#ruins").attr("checked", !0);
    jQuery("#ruins").change(function () {
        jQuery(this).prop("checked") ?
            set_cookie("ruins", 1) : set_cookie("ruins", 0)
    });
    c = get_cookie("savepos");
    "" == c && (c = 0);
    1 == c && jQuery("#savepos").attr("checked", !0);
    jQuery("#savepos").change(function () {
        jQuery(this).prop("checked") ? set_cookie("savepos", 1) : set_cookie("savepos", 0)
    });
    icon_hart = L.icon({
        iconUrl: "images/nodes/hart2.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_geschmeidig = L.icon({
        iconUrl: "images/nodes/geschmeidig2.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_abgelagert = L.icon({
        iconUrl: "images/nodes/abgelagert2.png",
        iconSize: [32,
            32
        ],
        iconAnchor: [16, 15]
    });
    icon_orichalcum = L.icon({
        iconUrl: "images/nodes/orichalcum.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_omnom = L.icon({
        iconUrl: "images/nodes/omnom.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_ancient = L.icon({
        iconUrl: "images/nodes/ancient.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_snow = L.icon({
        iconUrl: "images/nodes/snow.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_orr = L.icon({
        iconUrl: "images/nodes/orr.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_passion = L.icon({
        iconUrl: "images/nodes/passion.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_himbeere = L.icon({
        iconUrl: "images/nodes/himbeere.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_wp = L.icon({
        iconUrl: "images/waypoint.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_dungeon = L.icon({
        iconUrl: "images/dungeon.png",
        iconSize: [32, 32],
        iconAnchor: [16, 15]
    });
    icon_skillpoint = L.icon({
        iconUrl: "images/skillpoint.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_heart = L.icon({
        iconUrl: "images/heart.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_poi = L.icon({
        iconUrl: "images/poi.png",
        iconSize: [32, 32],
        iconAnchor: [16, 16]
    });
    icon_vista = L.icon({
        iconUrl: "images/vista.png",
        iconSize: [32, 32],
        iconAnchor: [16, 18]
    });
    icon_jp = L.icon({
        iconUrl: "images/jp.png",
        iconSize: [32, 32],
        iconAnchor: [18, 17]
    });
    icon_explorator = L.icon({
        iconUrl: "images/explorator-32.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_dragon_prepare = L.icon({
        iconUrl: "images/dragon-prepare-32.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_dragon_active = L.icon({
        iconUrl: "images/dragon-active-32.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_temple_prepare = L.icon({
        iconUrl: "images/tempel-contested.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_temple_last = L.icon({
        iconUrl: "images/tempel-contested-red.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_temple_open = L.icon({
        iconUrl: "images/tempel-free-green.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_dragon_part = L.icon({
        iconUrl: "images/dragon-part.png",
        iconSize: [25, 25],
        iconAnchor: [12, 13]
    });
    icon_dragon10 = L.icon({
        iconUrl: "images/dragon10.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_dragon20 =
        L.icon({
            iconUrl: "images/dragon20.png",
            iconSize: [32, 32],
            iconAnchor: [15, 16]
        });
    icon_dragon30 = L.icon({
        iconUrl: "images/dragon30.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_dragon40 = L.icon({
        iconUrl: "images/dragon40.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_dragon50 = L.icon({
        iconUrl: "images/dragon50.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_dragon60 = L.icon({
        iconUrl: "images/dragon60.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_dragon70 = L.icon({
        iconUrl: "images/dragon70.png",
        iconSize: [32,
            32
        ],
        iconAnchor: [15, 16]
    });
    icon_dragon80 = L.icon({
        iconUrl: "images/dragon80.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_dragon90 = L.icon({
        iconUrl: "images/dragon90.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_marker = L.icon({
        iconUrl: "images/flag.png",
        iconSize: [64, 64],
        iconAnchor: [30, 55],
        shadowUrl: "images/flag-shadow.png",
        shadowSize: [62, 37],
        shadowAnchor: [7, 32]
    });
    icon_event_simple = L.icon({
        iconUrl: "images/event-32.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_event_group = L.icon({
        iconUrl: "images/groupevent-32.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    icon_event_suche = L.icon({
        iconUrl: "images/suche.png",
        iconSize: [32, 32],
        iconAnchor: [15, 16]
    });
    jQuery("#updatebutton").click(tryupdate);
    geosettings.wp = jQuery("#layer-wp").prop("checked");
    geosettings.poi = jQuery("#layer-poi").prop("checked");
    geosettings.vista = jQuery("#layer-vista").prop("checked");
    geosettings.skillpoint = jQuery("#layer-skillpoint").prop("checked");
    geosettings.heart = jQuery("#layer-heart").prop("checked");
    geosettings.jp = jQuery("#layer-jp").prop("checked");
    geosettings.explorator = jQuery("#layer-explorator").prop("checked");
    geosettings.dungeon = jQuery("#layer-dungeon").prop("checked");
    jQuery("#layer-event input").change(event_layer);
    windowresize();
    map = (new L.map("map", {
        zoomControl: !embedded,
        keyboard: !embedded,
        dragging: !embedded,
        touchZoom: !embedded,
        scrollWheelZoom: !embedded,
        doubleCleckZoom: !embedded,
        boxZoom: !embedded
    })).setView([0, 0], 2);
    tiles = L.tileLayer("tiles/{z}-{x}-{y}.jpg", {
        attribution: 'Kartendaten &copy; <a href="http://www.guildwars2.com">ArenaNet</a>, <a href="http://www.wyld-bunch.de/datenschutzerklaerung/">Datenschutz</a>',
        maxZoom: 7,
        minZoom: 2,
        continuousWorld: !1,
        noWrap: !0
    });
    tiles.addTo(map);
    map.attributionControl.setPrefix(!1);
    map.on("click", mapClick);
    map.on("move", mapMove);
    map.on("viewreset", mapZoom);
    map.on("moveEnd", mapMove);
    map.on("move", save_position);
    map.on("zoomstart", zoomstart);
    jQuery("#mapselector").change(function () {
        toggle_tiles($(this).val())
    });
    embedded ? parse_startparameter() : (checklogin(), jQuery("#menu").show(), jQuery("#menu-accordion").accordion({
        heightStyle: "fill",
        active: 1
    }), jQuery("#praecat").accordion({
        heightStyle: "content",
        active: 1
    }), jQuery("#loginbutton").click(dologin), jQuery("#logoutbutton").click(dologout), jQuery("#username").keydown(function (a) {
        13 == a.which && jQuery("#passwort").focus()
    }), jQuery("#passwort").keydown(function (a) {
        13 == a.which && dologin()
    }));
    jQuery("#searchfield").keydown(function (a) {
        13 == a.which && start_search()
    });
    jQuery.get("https://api.guildwars2.com/v1/world_names.json?lang=de", generate_serverlist);
    jQuery.post("ajax.php", {
        action: "getzones"
    }, function (a) {
        if (a = eval("(" + a + ")")) {
            for (var c = 0; c < a.region.length; c++) zones.push([a.region[c].map,
                a.region[c].zone, 0, eval(a.region[c].bound)
            ]);
            display_zones()
        }
    });
    layer_wp = new L.layerGroup;
    layer_poi = new L.layerGroup;
    layer_vista = new L.layerGroup;
    layer_skillpoint = new L.layerGroup;
    layer_heart = new L.layerGroup;
    layer_jp = new L.layerGroup;
    layer_explorator = new L.layerGroup;
    layer_dungeon = new L.layerGroup;
    layer_delpath = (new L.layerGroup).addTo(map);
    layer_prequests = (new L.layerGroup).addTo(map);
    layer_events = (new L.layerGroup).addTo(map);
    layer_nodes = (new L.layerGroup).addTo(map);
    jQuery("#layer-geo input").change(geo_layer);
    tiles.on("loading", function () {
        mapload = 1
    });
    tiles.on("load", function () {
        mapload = 0;
        mapgoto()
    });
    map.on("zoomend", mapgoto);
    map.on("zoomend", save_position);
    gadda()
}

function gadda() {
    var a = jQuery("#gadcontainer");
    0 == a.length && jQuery("#menu").remove();
    a = jQuery("#gadcontainer").children("ins");
    0 == a.length && (gutts = 0, dotherot = 1, jQuery("#gadcontainer").html('<div style="text-align: center;padding-top:20px;"><a href="http://www.heise.de/Adblocker-auf-heise-online-1164703.html" onclick="window.open(this.href);return false;">Link gegen Blink</a><div>'), window.setInterval(gaddandom, 500))
}

function gaddandom() {
    1 == dotherot ? jQuery("#gadcontainer").css({
        "background-color": "#f00"
    }) : jQuery("#gadcontainer").css({
        "background-color": "#fff"
    });
    dotherot = 1 - dotherot
}

function zoomstart() {}

function save_position() {
    map_active && (1 != mapmode && 2 == mapmode) && (set_cookie("wvw_pos", map.getCenter().toString()), set_cookie("wvw_zoom", map.getZoom()))
}

function parse_startparameter(a) {
    var c = 1,
        b = !1,
        e = !0;
    a = ("undefined" != typeof a ? a : window.location.search.substr(1)).split("&");
    for (var d = 0; d < a.length; d++) {
        var g = a[d].split("=");
        "goto" == g[0] && (b = !0, e = !1, deactivate_map(), pos = decodeURI(g[1]).split(","), showmarker = L.marker(pos, {
            icon: icon_marker,
            title: "angegebene Position"
        }).addTo(map), added_marker(), "undefined" != typeof zoomtimer && window.clearTimeout(zoomtimer), mapload || window.setTimeout(mapgoto, 2E3));
        "server" == g[0] && (server = parseInt(g[1]));
        "map" == g[0] && ("pve" ==
            g[1] ? c = 1 : "wvw" == g[1] ? (c = 2, jQuery('#layer-mapchoice option[value="2"]').attr("selected", !0)) : notify_user('Ung\u00fcltiger Wert f\u00fcr Parameter "map", zul\u00e4ssig sind "pve"" und "wvw"'));
        "sound" == g[0] && ("full" == g[1] ? window.setTimeout(function () {
            jQuery("#event-speak").attr("checked", !0);
            jQuery("#event-alarm").attr("checked", !1);
            playSounds = vorlesen = !0
        }, 2E3) : "simple" == g[1] ? window.setTimeout(function () {
            jQuery("#event-speak").attr("checked", !1);
            jQuery("#event-alarm").attr("checked", !0);
            vorlesen = !1;
            playSounds = !0
        }, 2E3) : "none" == g[1] ? window.setTimeout(function () {
            jQuery("#event-speak").attr("checked", !1);
            jQuery("#event-alarm").attr("checked", !1);
            playSounds = vorlesen = !1
        }, 2E3) : notify_user('Ung\u00fcltiger Wert f\u00fcr Parameter "sound", zul\u00e4ssig sind "full", "simple" und "none"'));
        "special" == g[0] && (b = !0, e = !1, forced_login && !angemeldet ? delay_parameter += "&special=" + g[1] : (window.setTimeout("gotospecial(" + g[1] + ")", 2E3), deactivate_map()));
        "marker" == g[0] && (b = !0, e = !1, forced_login && !angemeldet ? delay_parameter +=
            "&marker=" + g[1] : goto_marker(g[1]));
        if ("events" == g[0]) {
            var b = !0,
                h = "1" == g[1] || "true" == g[1];
            forced_login && !angemeldet ? delay_parameter += "events=" + h : h && window.setTimeout(function () {
                loadevents = !0;
                jQuery("#event-kiste").attr("checked", !0);
                events[1] = 1
            }, 2E3)
        }
        "nodes" == g[0] && (b = !0, h = "1" == g[1] || "true" == g[1], forced_login && !angemeldet ? delay_parameter += "nodes=" + nodes : h && window.setTimeout(function () {
            jQuery("#layer-nodes").attr("checked", !0);
            add_nodes()
        }, 8E3));
        "temple" == g[0] && (b = !0, forced_login && !angemeldet ? delay_parameter +=
            "&temple=" + g[1] : "all" == g[1] ? window.setTimeout(function () {
                loadevents = !0;
                jQuery("#event-tempel").attr("checked", !0);
                jQuery("#event-tempel-open").attr("checked", !1);
                events[2] = !0;
                events[3] = !1
            }, 2E3) : "last" == g[1] ? window.setTimeout(function () {
                loadevents = !0;
                jQuery("#event-tempel").attr("checked", !1);
                jQuery("#event-tempel-open").attr("checked", !0);
                events[2] = !1;
                events[3] = !0
            }, 2E3) : notify_user('Ung\u00fcltiger Wert f\u00fcr Parameter "temple", zul\u00e4ssig sind "all" und "last"'));
        "active_events" == g[0] && (b = !0, forced_login && !angemeldet ? delay_parameter += "&active-events=" + g[1] : "all" == g[1] ? window.setTimeout(function () {
            loadevents = !0;
            jQuery("#event-simple").attr("checked", !0);
            jQuery("#event-group").attr("checked", !1);
            events[4] = !0;
            events[5] = !1
        }, 2E3) : "group" == g[1] ? window.setTimeout(function () {
            loadevents = !0;
            jQuery("#event-simple").attr("checked", !1);
            jQuery("#event-group").attr("checked", !0);
            events[4] = !1;
            events[5] = !0
        }, 2E3) : notify_user('Ung\u00fcltiger Wert f\u00fcr Parameter "active_events", zul\u00e4ssig sind "all" und "group"'));
        "open" == g[0] && (b = !0, e = !1, forced_login && !angemeldet ? delay_parameter += "&open=" + g[1] : autoopen = decodeURI(g[1]));
        "search" == g[0] && (b = !0, e = !1, forced_login && !angemeldet ? delay_parameter += "&search=" + g[1] : (autoopen = "suche", autosuche = decodeURI(g[1])))
    }
    forced_login && !angemeldet || window.setTimeout(function () {
        loadevents && serverdataloaded && (loadevents = !1, check_event_timer_state())
    }, 3E3);
    2 != c || b || toggle_tiles(2);
    e && !embedded && changelog();
    forced_login && (!angemeldet && 0 < delay_parameter.length) && (notify_user("F\u00fcr diese Funktion ist die Anmeldung erforderlich."),
        autoopen = "anmelden")
}

function activate_layer(a) {
    1 == a && (jQuery("#layer-wp").prop("checked", !0), geosettings.wp = !0);
    2 == a && (jQuery("#layer-poi").prop("checked", !0), geosettings.poi = !0);
    3 == a && (jQuery("#layer-vista").prop("checked", !0), geosettings.vista = !0);
    4 == a && (jQuery("#layer-skillpoint").prop("checked", !0), geosettings.skillpoint = !0);
    5 == a && (jQuery("#layer-heart").prop("checked", !0), geosettings.heart = !0);
    6 == a && (jQuery("#layer-jp").prop("checked", !0), geosettings.jp = !0);
    7 == a && (jQuery("#layer-dungeon").prop("checked", !0), geosettings.dungeon = !0);
    8 == a && (jQuery("#layer-explorator").prop("checked", !0), geosettings.explorator = !0)
}

function added_marker() {
    embedded || ("undefined" == typeof show_remove_marker && (show_remove_marker = !1), show_remove_marker || (show_remove_marker = !0, window.setTimeout(function () {
        jQuery("#map").append('<div id="remove_marker"><a href="javascript:delete_marker();" onclick="blur();"><img src="images/adelete.png" width="16" height="16" alt="delete" /> Markierungen l\u00f6schen</a></div>');
        jQuery("#remove_marker").css({
            opacity: 0
        }).show().animate({
            opacity: 0.3
        }, 5E3).mouseover(function () {
            jQuery(this).stop(!0).animate({
                opacity: 1
            })
        }).mouseout(function () {
            jQuery(this).animate({
                    opacity: 0.3
                },
                5E3)
        })
    }, 6E4)))
}

function delete_marker() {
    show_remove_marker && (show_remove_marker = !1, jQuery("#remove_marker").stop(!0).off("mouseover mouseout").animate({
        opacity: 0
    }, 2E3), window.setTimeout(function () {
        jQuery("#remove_marker").remove()
    }, 3E3), "undefined" != typeof aniPaths && aniPaths.clearLayers(), "undefined" != typeof aniMarker && aniMarker.clearLayers(), map.removeLayer(showmarker), map.closePopup())
}

function goto_marker(a) {
    deactivate_map();
    menuopen && togglemenu();
    2 != map.getZoom() || mapload || window.setTimeout(mapgoto, 2E3);
    map.removeLayer(showmarker);
    map.setView([0, 0], 2);
    pos = -1;
    movingstep = 0;
    jQuery.post("ajax.php", {
        action: "getwp",
        id: a
    }, function (a) {
        if (a = eval("(" + a + ")")) a.result ? (pos = [a.marker.lati, a.marker.longi], searchformarker = a.marker.id, activate_layer(a.marker.type), 10 == a.marker.type && (specialmarker = a.marker.id), deletemarker = L.marker(pos, {
                icon: icon_marker,
                title: "angegebene Position"
            }).addTo(map),
            "undefined" != typeof zoomtimer && window.clearTimeout(zoomtimer)) : (pos = 0, notify_user("Angegebener Marker existiert nicht."), activate_map())
    })
}

function set_title(a) {
    var c = "",
        c = "undefined" == typeof a ? 2 == mapmode ? "WvW-Map | Wyld Bunch" : "Map | Wyld Bunch" : a + " | Wyld-Bunch-Map-Tool";
    document.title = c
}

function start_search() {
    jQuery("#results").html("suche...");
    var a = jQuery("#searchfield").val();
    jQuery.post("ajax.php", {
        action: "search",
        term: a
    }, function (a) {
        if (a = eval("(" + a + ")"))
            if ("none" == a.result && (jQuery("#results").html("keine Ergebnisse"), jQuery("#searchfield").select()), "ok" == a.result) {
                var b = a.resultlist.length;
                1 == b && (1 == a.resultlist[0].db ? goto_marker(a.resultlist[0].id) : 2 == a.resultlist[0].db ? gotospecial(a.resultlist[0].id) : 3 == a.resultlist[0].db && newmove(a.resultlist[0].lati, a.resultlist[0].longi, !1));
                for (var e = "<ul>", d = 0; d < b; d++) 1 == a.resultlist[d].db ? e += '<li><a href="' + window.location.protocol + "//" + window.location.host + window.location.pathname + "?marker=" + a.resultlist[d].id + '" onclick="goto_marker(' + a.resultlist[d].id + ');return false;"><img width="16" height="16" src="' + get_icon(a.resultlist[d].type) + '" /> ' + a.resultlist[d].name + "</a></li>" : 2 == a.resultlist[d].db ? e += '<li><a href="' + window.location.protocol + "//" + window.location.host + window.location.pathname + "?special=" + a.resultlist[d].id + '" onclick="gotospecial(' +
                    a.resultlist[d].id + ');return false;"><img width="16" height="16" src="' + get_icon(11) + '" /> ' + a.resultlist[d].name + "</a></li>" : 3 == a.resultlist[d].db && (e += '<li><a href="' + window.location.protocol + "//" + window.location.host + window.location.pathname + "?goto=" + a.resultlist[d].lati + "," + a.resultlist[d].longi + '" onclick="newmove(' + a.resultlist[d].lati + "," + a.resultlist[d].longi + ',false);return false;"><img width="16" height="16" src="' + get_icon(13) + '" /> ' + a.resultlist[d].name + "</a></li>");
                e += "</ul>";
                jQuery("#results").html(e)
            }
    })
}

function geo_layer(a) {
    a = map.getZoom();
    "layer-wp" == jQuery(this).attr("id") && (jQuery(this).prop("checked") ? (!map.hasLayer(layer_wp) && 5 <= a && layer_wp.addTo(map), geosettings.wp = 1) : (map.hasLayer(layer_wp) && map.removeLayer(layer_wp), geosettings.wp = 0));
    "layer-poi" == jQuery(this).attr("id") && (jQuery(this).prop("checked") ? (!map.hasLayer(layer_poi) && 5 <= a && layer_poi.addTo(map), geosettings.poi = 1) : (map.hasLayer(layer_poi) && map.removeLayer(layer_poi), geosettings.poi = 0));
    "layer-vista" == jQuery(this).attr("id") && (jQuery(this).prop("checked") ?
        (!map.hasLayer(layer_vista) && 5 <= a && layer_vista.addTo(map), geosettings.vista = 1) : (map.hasLayer(layer_vista) && map.removeLayer(layer_vista), geosettings.vista = 0));
    "layer-skillpoint" == jQuery(this).attr("id") && (jQuery(this).prop("checked") ? (!map.hasLayer(layer_skillpoint) && 5 <= a && layer_skillpoint.addTo(map), geosettings.skillpoint = 1) : (map.hasLayer(layer_skillpoint) && map.removeLayer(layer_skillpoint), geosettings.skillpoint = 0));
    "layer-heart" == jQuery(this).attr("id") && (jQuery(this).prop("checked") ? (!map.hasLayer(layer_heart) &&
        5 <= a && layer_heart.addTo(map), geosettings.heart = 1) : (map.hasLayer(layer_heart) && map.removeLayer(layer_heart), geosettings.heart = 0));
    "layer-dungeon" == jQuery(this).attr("id") && (jQuery(this).prop("checked") ? (!map.hasLayer(layer_dungeon) && 5 <= a && layer_dungeon.addTo(map), geosettings.dungeon = 1) : (map.hasLayer(layer_dungeon) && map.removeLayer(layer_dungeon), geosettings.dungeon = 0));
    "layer-jp" == jQuery(this).attr("id") && (jQuery(this).prop("checked") ? (!map.hasLayer(layer_jp) && 5 <= a && layer_jp.addTo(map), geosettings.jp =
        1) : (map.hasLayer(layer_jp) && map.removeLayer(layer_jp), geosettings.jp = 0));
    "layer-explorator" == jQuery(this).attr("id") && (jQuery(this).prop("checked") ? (!map.hasLayer(layer_explorator) && 5 <= a && layer_explorator.addTo(map), geosettings.explorator = 1) : (map.hasLayer(layer_explorator) && map.removeLayer(layer_explorator), geosettings.explorator = 0));
    "layer-nodes" == jQuery(this).attr("id") && (map.closePopup(), jQuery("#layer-nodes").prop("checked") ? add_nodes() : layer_nodes.clearLayers());
    load_marker()
}

function mapZoom(a) {
    jQuery(".ui-tooltip").mouseover(function () {
        jQuery(this).remove()
    });
    5 <= map.getZoom() ? (!map.hasLayer(layer_wp) && geosettings.wp && layer_wp.addTo(map), !map.hasLayer(layer_poi) && geosettings.poi && layer_poi.addTo(map), !map.hasLayer(layer_vista) && geosettings.vista && layer_vista.addTo(map), !map.hasLayer(layer_skillpoint) && geosettings.skillpoint && layer_skillpoint.addTo(map), !map.hasLayer(layer_heart) && geosettings.heart && layer_heart.addTo(map), !map.hasLayer(layer_jp) && geosettings.jp && layer_jp.addTo(map), !map.hasLayer(layer_explorator) && geosettings.explorator && layer_explorator.addTo(map), !map.hasLayer(layer_dungeon) && geosettings.dungeon && layer_dungeon.addTo(map)) : (map.removeLayer(layer_wp), map.removeLayer(layer_poi), map.removeLayer(layer_vista), map.removeLayer(layer_skillpoint), map.removeLayer(layer_heart), map.removeLayer(layer_jp), map.removeLayer(layer_explorator), map.removeLayer(layer_dungeon))
}

function newmove(a, c, b) {
    "undefined" == typeof b && (b = !0);
    deactivate_map();
    "undefined" != typeof aniMarker && aniMarker.clearLayers();
    "undefined" != typeof aniPaths && aniPaths.clearLayers();
    showmarker && map.hasLayer(showmarker) && map.removeLayer(showmarker);
    menuopen && togglemenu();
    2 == map.getZoom() && window.setTimeout(mapgoto, 2E3);
    map.setView([0, 0], 2);
    pos = [a, c];
    b ? showmarker = L.marker(pos, {
        icon: icon_marker,
        title: "angegebene Position"
    }).addTo(map) : deletemarker = L.marker(pos, {
        icon: icon_marker,
        title: "angegebene Position"
    }).addTo(map);
    movingstep = 0
}

function mapgoto() {
    if (pos && !mapload)
        if (-1 == pos) setTimeout(function () {
            mapgoto()
        }, 500);
        else {
            var a;
            movingstep++;
            2 > poszoom && (poszoom = 2);
            1 == movingstep ? (a = 3, poszoom <= a && (a = poszoom, movingstep = 3), "undefined" != typeof zoomtimer && window.clearTimeout(zoomtimer), 2 == poszoom && setTimeout(mapgoto, 4E3), zoomtimer = setTimeout(function () {
                map.setView(pos, a)
            }, 2E3)) : 2 == movingstep ? (a = 5, poszoom <= a && (a = poszoom, movingstep = 3), "undefined" != typeof zoomtimer && window.clearTimeout(zoomtimer), zoomtimer = setTimeout(function () {
                    map.setView(pos, a)
                },
                2E3)) : 3 == movingstep ? (a = 7, poszoom < a && (a = poszoom), "undefined" != typeof zoomtimer && window.clearTimeout(zoomtimer), zoomtimer = setTimeout(function () {
                map.setView(pos, a)
            }, 2E3)) : 4 == movingstep && (activate_map(), "undefined" != typeof deletemarker && map.removeLayer(deletemarker), poszoom = 7, "undefined" != typeof clickmarker && (embedded || markerclick({
                data: {
                    id: searchformarker,
                    type: 10,
                    marker: clickmarker
                }
            }), delete clickmarker), "undefined" == typeof posmarker && "undefined" == typeof pospaths || animate_paths(1), pos = 0)
        }
}

function animate_paths(a) {
    var c = 0;
    "undefined" != typeof posmarker && (c = posmarker.length);
    var b = 0;
    "undefined" != typeof pospaths && (b = pospaths.length);
    for (var e = 0, d = 1; d <= b; d++) e += pospaths[d - 1].length;
    1 == a && ("undefined" != typeof aniMarker && aniMarker.clearLayers(), "undefined" != typeof aniPaths && aniPaths.clearLayers(), aniMarker = new L.layerGroup, aniPaths = new L.layerGroup, aniMarker.addTo(map), aniPaths.addTo(map));
    a <= c && (m = L.marker([posmarker[a - 1].lati, posmarker[a - 1].longi], {
            icon: icon_marker,
            title: posmarker[a - 1].name
        }).addTo(aniMarker),
        jQuery(m).on("click", {
            id: posmarker[a - 1].id,
            type: posmarker[a - 1].type,
            special: active_special,
            special_name: active_special_name,
            marker: m
        }, markerclick));
    if (a <= e)
        for (aniPaths.clearLayers(), d = b = 0; d < pospaths.length; d++) {
            if (pospaths[d].length + b <= a) L.polyline(pospaths[d], {
                color: get_edit_color(d),
                clickable: !1
            }).addTo(aniPaths);
            else if (a > b) {
                for (var g = [], h = 0; h < a - b; h++) g[h] = pospaths[d][h];
                L.polyline(g, {
                    color: get_edit_color(d),
                    clickable: !1
                }).addTo(aniPaths)
            }
            b += pospaths[d].length
        }
    a++;
    a <= c || a <= e ? window.setTimeout("animate_paths(" +
        a + ")", 75) : (delete posmarker, delete pospaths, added_marker())
}

function mapMove(a) {
    a = map.getZoom();
    if (1 == mapmode)
        if (5 <= a && (window.clearTimeout(movetimer), movetimer = window.setTimeout(load_marker, 500)), 6 <= a) {
            a = !1;
            for (var c = 0; c < zones.length; c++) {
                var b = new L.LatLngBounds(zones[c][3]);
                b.contains(map.getCenter()) && (map.attributionControl.setPrefix("<strong>" + zones[c][0] + "</strong>"), a = !0, set_title(zones[c][0]))
            }
            a || (map.attributionControl.setPrefix(!1), set_title())
        } else map.attributionControl.setPrefix(!1), set_title();
        else if (2 == mapmode)
        if (4 <= a) {
            a = !1;
            for (c = 0; c < wvwzones.length; c++) b =
                new L.LatLngBounds(wvwzones[c][3]), b.contains(map.getCenter()) && (!1 == limitarea && addlog("Schr\u00e4nke Meldungen auf aktuelles Gebiet ein.", void 0, !1), limitarea = b, map.attributionControl.setPrefix("<strong>" + wvwzones[c][0] + " (" + wvwzones[c][1] + ")</strong>"), a = !0, set_title(wvwzones[c][0] + " (" + wvwzones[c][1] + ")"));
            a || (map.attributionControl.setPrefix(!1), !1 != limitarea && addlog("Zeige alle WvW-Meldungen", void 0, !1), limitarea = !1, set_title())
        } else map.attributionControl.setPrefix(!1), !1 != limitarea && addlog("Zeige alle WvW-Meldungen"),
    limitarea = !1, set_title()
}

function mapClick(a) {
    if (!embedded && 2 != mapmode && allowclick)
        if (multiopen && 1 == selected_tool) {
            var c = String(a.latlng),
                c = c.replace(/\s+/g, "");
            a = c.substr(7, c.length - 8);
            marker_mask({
                action: "add",
                pos: a,
                type: 12
            })
        } else if (multiopen && 2 == selected_tool) edit_path_add(a.latlng);
    else if (2 == map.getZoom())
        if (1 == mapmode)
            for (c = 0; c < zones.length; c++) {
                var b = new L.LatLngBounds(zones[c][3]);
                b.contains(a.latlng) && (b = new L.LatLngBounds(zones[c][3]), map.fitBounds(b))
            } else {
                if (2 == mapmode)
                    for (c = 0; c < wvwzones.length; c++) b = new L.LatLngBounds(wvwzones[c][3]),
                b.contains(a.latlng) && (b = new L.LatLngBounds(wvwzones[c][3]), map.fitBounds(b))
            } else {
                c = String(a.latlng);
                c = c.replace(/\s+/g, "");
                b = "";
                if (1 == rights.add || 1 == rights.addnodes || 1 == rights.resetnodes) {
                    b += "<p><strong>An dieser Position hinzuf\u00fcgen:</strong><br />";
                    if (1 == rights.add) {
                        1 == rights.addwp && (b = geosettings.wp ? b + ("<a href=\"javascript:marker_mask({action: 'add', pos: '" + c.substr(7, c.length - 8) + '\', type: 1});"><img src="images/waypoint32.png" /></a>') : b + '<img style="opacity:0.5;" title="Um neue Wegmarken anzulegen muss der entsprechende Layer aktiviert sein." src="images/waypoint32.png" />');
                        1 == rights.addpoi && (b = geosettings.poi ? b + ("<a href=\"javascript:marker_mask({action: 'add', pos: '" + c.substr(7, c.length - 8) + '\', type: 2});"><img src="images/poi.png" /></a>') : b + '<img style="opacity:0.5;" title="Um neue Sehensw\u00fcrdigkeiten anzulegen muss der entsprechende Layer aktiviert sein." src="images/poi.png" />');
                        1 == rights.addvista && (b = geosettings.vista ? b + ("<a href=\"javascript:marker_mask({action: 'add', pos: '" + c.substr(7, c.length - 8) + '\', type: 3});"><img src="images/vista.png" /></a>') :
                            b + '<img style="opacity:0.5;" title="Um neue Vistae anzulegen muss der entsprechende Layer aktiviert sein." src="images/vista.png" />');
                        1 == rights.addskillpoint && (b = geosettings.skillpoint ? b + ("<a href=\"javascript:marker_mask({action: 'add', pos: '" + c.substr(7, c.length - 8) + '\', type: 4});"><img src="images/skillpoint.png" /></a>') : b + '<img style="opacity:0.5;" title="Um neue Skillpoints anzulegen muss der entsprechende Layer aktiviert sein." src="images/skillpoint.png" />');
                        1 == rights.addheart && (b = geosettings.heart ?
                            b + ("<a href=\"javascript:marker_mask({action: 'add', pos: '" + c.substr(7, c.length - 8) + '\', type: 5});"><img src="images/heart.png" /></a>') : b + '<img style="opacity:0.5;" title="Um neue Herz-Quests anzulegen muss der entsprechende Layer aktiviert sein." src="images/heart.png" />');
                        1 == rights.addjp && (b = geosettings.jp ? b + ("<a href=\"javascript:marker_mask({action: 'add', pos: '" + c.substr(7, c.length - 8) + '\', type: 6});"><img src="images/jp.png" /></a>') : b + '<img style="opacity:0.5;" title="Um neue Sprung-R\u00e4tsel anzulegen muss der entsprechende Layer aktiviert sein." src="images/jp.png" />');
                        1 == rights.addexplorator && (b = geosettings.explorator ? b + ("<a href=\"javascript:marker_mask({action: 'add', pos: '" + c.substr(7, c.length - 8) + '\', type: 8});"><img src="images/explorator-32.png" /></a>') : b + '<img style="opacity:0.5;" title="Um neue Explorator-Marken anzulegen muss der entsprechende Layer aktiviert sein." src="images/explorator-32.png" />');
                        1 == rights.adddungeon && (b = geosettings.dungeon ? b + ("<a href=\"javascript:marker_mask({action: 'add', pos: '" + c.substr(7, c.length - 8) + '\', type: 7});"><img src="images/dungeon.png" /></a>') :
                            b + '<img style="opacity:0.5;" title="Um neue Dungeons anzulegen muss der entsprechende Layer aktiviert sein." src="images/dungeon.png" />');
                        if (1 == rights.single || 1 == rights.multi) b += '<span style="padding-left:30px;"> </span>';
                        1 == rights.single && (b += "<a href=\"javascript:marker_mask({action: 'add', pos: '" + c.substr(7, c.length - 8) + '\', type: 10});" title="Marker, der in einer der Kategorien gespeichert wird" ><img src="images/little-flag.png" /></a>');
                        1 == rights.multi && (b += '<a href="javascript:start_multi();" title="Multi-Marker, Pathes und \u00e4hnliches. Wird in Kategorien gespeichert."><img src="images/multi.png" /></a>')
                    }
                    1 !=
                        rights.add || 1 != rights.addnodes && 1 != rights.resetnodes || (b += '<span style="padding-left:30px;"> </span>');
                    1 == rights.addnodes && (jQuery("#layer-nodes").prop("checked") ? (b += '<a href="javascript:addnode(1, ' + c.substr(7, c.length - 8) + ');" title="geschmeidiger Holzblock" ><img src="images/nodes/geschmeidig2.png" /></a>', b += '<a href="javascript:addnode(2, ' + c.substr(7, c.length - 8) + ');" title="abgelagerter Holzblock" ><img src="images/nodes/abgelagert2.png" /></a>', b += '<a href="javascript:addnode(3, ' + c.substr(7,
                        c.length - 8) + ');" title="harter Holzblock" ><img src="images/nodes/hart2.png" /></a>') : (b += '<img style="opacity:0.5;" title="Zum Setzen neuer Nodes m\u00fcssen die Crafting-Materialien im Men\u00fc erst aktiviert werden" src="images/nodes/geschmeidig2.png" /><img style="opacity:0.5;" title="Zum Setzen neuer Nodes m\u00fcssen die Crafting-Materialien im Men\u00fc erst aktiviert werden" src="images/nodes/abgelagert2.png" />', b += '<img style="opacity:0.5;" title="Zum Setzen neuer Nodes m\u00fcssen die Crafting-Materialien im Men\u00fc erst aktiviert werden" src="images/nodes/hart2.png" />'));
                    1 == rights.resetnodes && (b = jQuery("#layer-nodes").prop("checked") ? b + '<a href="javascript:resetallnodes();" title="alle Sammelnodes auf allen Servern l\u00f6schen" ><img src="images/adelete.png" /></a>' : b + '<img style="opacity:0.5;" title="Vor dem L\u00f6schen m\u00fcssen erst die Crafting-Materialien im Men\u00fc aktiviert werden" src="images/adelete.png" />');
                    b += "</p>"
                }
                var b = b + "<p><strong>Diese Position als Link versenden:</strong><br />",
                    e = window.location.protocol + "//" + window.location.host + window.location.pathname +
                        "?goto=" + encodeURI(c.substr(7, c.length - 8)),
                    b = b + ('<input style="width:400px;" type="text" id="link1" value="' + e + '" /></p>'),
                    b = b + "<div>",
                    b = b + '<div style="float:left;width:270px;">',
                    b = b + ('<p style="margin-top:0;"><strong>Diese Position f\u00fcrs Forum:</strong><br /><input style="width:250px" type="text" id="link2" value="[url=' + e + ']Karte[/url]" /></p>'),
                    b = b + "</div>",
                    b = b + ('<p><strong>Aktuelle Position:</strong><br /><input id="positionbox" style="width:125px;" type="text" value="' + c.substr(7, c.length -
                        8) + '" /></p>'),
                    b = b + "</div>";
                L.popup({
                    maxWidth: 400
                }).setLatLng(a.latlng).setContent(b).openOn(map);
                jQuery("#link1").click(function () {
                    jQuery("#link1").select()
                });
                jQuery("#link2").click(function () {
                    jQuery("#link2").select()
                });
                jQuery("#positionbox").click(function () {
                    jQuery("#positionbox").select()
                })
            }
}

function resetallnodes() {
    jQuery.post("ajax.php", {
        action: "getlastreset"
    }, function (a) {
        a = eval("(" + a + ")");
        a = "<p>Letzter Reset vor " + timediff(new Date(1E3 * a.time)) + " durch \u00bb" + a.user + "\u00ab.</p>";
        jQuery("#dialog").html("<p>Mit dieser Funktion werden alle Sammelnodes auf allen Server-Maps gel\u00f6scht! Sie sollte nur nach Serverneustarts und/oder Patches verwendet werden, wenn auf allen Servern neue Positionen vorliegen m\u00fcssen. (Betrifft nicht die Orichalcum- und anderen Nodes, die automatisch von gw2nodes bezogen werden.)</p>" + a).dialog({
            title: "Alle Sammelnodes aller Server l\u00f6schen?",
            resizable: !1,
            modal: !0,
            width: 500,
            buttons: [{
                text: "nicht l\u00f6schen",
                click: function () {
                    jQuery(this).dialog("close")
                }
            }, {
                text: "ja, wirklich l\u00f6schen",
                click: function () {
                    jQuery(this).dialog("close");
                    map.closePopup();
                    jQuery.post("ajax.php", {
                        action: "resetnodes"
                    }, function (a) {
                        1 == eval("(" + a + ")").result && (notify_user("Alle Sammelnodes von allen Servern gel\u00f6scht."), add_nodes())
                    })
                }
            }],
            show: {
                effect: "drop",
                duraction: 1E3
            },
            hide: {
                effect: "explode",
                duration: 1E3
            }
        })
    })
}

function addnode(a, c, b) {
    var e = jQuery("#world").val();
    map.closePopup();
    jQuery.post("ajax.php", {
        action: "addnode",
        type: a,
        lati: c,
        longi: b,
        server: e
    }, function (a) {
        a = eval("(" + a + ")");
        0 < a.result ? add_usernode_to_map([c, b], a.type, a.result, userid) : notify_user("Fehler beim Hinzuf\u00fcgen...")
    })
}

function edit_multi(a) {
    gotospecial(a);
    var c;
    jQuery.post("ajax.php", {
        action: "getspecial",
        id: a
    }, function (b) {
        if (b = eval("(" + b + ")")) {
            var e = 0;
            if ("undefined" != typeof b.marker) {
                posmarker = b.marker;
                for (var d = 0; d < b.marker.length; d++) 10 == b.marker[d].type && (e = b.marker[d].id)
            }
            e || start_multi();
            for (d = 0; d < catelements.length; d++)
                if (catelements[d].id == a) {
                    c = "<h1>Marker bearbeiten</h1>";
                    c += '<p><label for="ename">Titel: </label><input type="text" id="ename" value="' + catelements[d].name + '" /></p>';
                    c += '<p><label for="ecat1">Kategorie: </label>';
                    c += '<select id="ecat1">';
                    for (var g = 0; g < categories.length; g++) 0 == categories[g].parent && (c += '<option value="' + categories[g].id + '"' + (catelements[d].cat1 == categories[g].id ? ' selected="selected"' : "") + ">" + categories[g].name + "</option>");
                    c += "</select></p>";
                    c += '<p><label for="ecat2">Unterkategorie: </label>';
                    c += '<select id="ecat2">';
                    for (g = 0; g < categories.length; g++) categories[g].parent == catelements[d].cat1 && (c += '<option value="' + categories[g].id + '"' + (catelements[d].cat2 == categories[g].id ? ' selected="selected"' :
                        "") + ">" + categories[g].name + "</option>");
                    c += "</select></p>";
                    if (!e) {
                        c += '<p><label for="delpath">L\u00f6sche Pfad: <label>';
                        c += '<select id="delpath">';
                        c += '<option value="-1" selected="selected">keinen</option>';
                        if ("undefined" != typeof b.paths)
                            for (g = 0; g < b.paths.length; g++) c += '<option value="' + b.paths[g].id + '">Pfad ' + (g + 1) + "</option>";
                        delpaths = b.paths;
                        c += "</select></p>"
                    }
                    c += '<p><input type="button" id="edit_save_button" value="speichern" /> <input type="button" id="edit_delete_button" value="l\u00f6schen" /> <input type="button" id="edit_save_cancel" value="abbrechen" /></p>';
                    jQuery("#multi-name").html(c);
                    jQuery("#ecat1").change(refresh_edit_cat);
                    jQuery("#edit_save_cancel").click(close_both);
                    jQuery("#edit_delete_button").click({
                        id: a
                    }, delete_edit);
                    jQuery("#edit_save_button").click({
                        id: a,
                        single: e
                    }, update_edit);
                    jQuery("#delpath").change(update_del_path);
                    window.setTimeout(open_edit_save, 8E3)
                }
        }
    })
}

function update_del_path() {
    var a = jQuery("#delpath").val();
    layer_delpath.clearLayers();
    if (a && "undefined" != typeof delpaths)
        for (var c = 0; c < delpaths.length; c++)
            if (delpaths[c].id == a) {
                var b = eval("[" + delpaths[c].path + "]");
                L.polyline(b, {
                    color: "black",
                    clickable: !1,
                    opacity: 0.8
                }).addTo(layer_delpath)
            }
}

function close_both() {
    close_edit_save();
    abort_edit();
    layer_delpath.clearLayers();
    delete delpaths
}

function delete_edit(a) {
    layer_delpath.clearLayers();
    jQuery.post("ajax.php", {
        action: "deletespecial",
        id: a.data.id
    }, function (a) {
        (a = eval("(" + a + ")")) && "ok" == a.result && (notify_user("Eintrag erfolgreich gel\u00f6scht."), update_categories(), close_both())
    })
}

function update_edit(a) {
    layer_delpath.clearLayers();
    var c = jQuery("#ename").val(),
        b = jQuery("#ecat1").val(),
        e = jQuery("#ecat2").val(),
        d = jQuery("#delpath").val();
    a.data.single ? jQuery.post("ajax.php", {
        action: "editspecial",
        id: a.data.id,
        cat1: b,
        cat2: e,
        name: c
    }, function (a) {
        if (a = eval("(" + a + ")")) "ok" == a.result ? (update_categories(), close_both()) : update_categories(), notify_user(a.message)
    }) : (a = "&id=" + a.data.id + "&name=" + encodeURI(c) + "&cat1=" + jQuery("#ecat1").val() + "&cat2=" + jQuery("#ecat2").val() + "&delpath=" + d, a +=
        encode_edit_settings(), jQuery.post("ajax.php", "action=edit-multi" + a, function (a) {
            if (a = eval("(" + a + ")")) "error" == a.result && notify_user(a.message), "ok" == a.result && (notify_user(a.message), close_edit_save(), abort_edit(), update_categories())
        }))
}

function gotospecial(a) {
    deactivate_map();
    menuopen && togglemenu();
    multiopen && abort_edit();
    map.closePopup();
    map.removeLayer(showmarker);
    jQuery.post("ajax.php", {
        action: "getspecial",
        id: a
    }, function (c) {
        if (c = eval("(" + c + ")")) {
            var b = new L.LatLngBounds,
                e = 0;
            if ("undefined" != typeof c.marker) {
                posmarker = c.marker;
                for (var d = 0; d < c.marker.length; d++) {
                    if (10 == c.marker[d].type) var e = c.marker[d].id,
                    g = L.marker([c.marker[d].lati, c.marker[d].longi], {
                        icon: icon_marker,
                        title: c.marker[d].name
                    });
                    b.extend([c.marker[d].lati, c.marker[d].longi])
                }
            }
            if ("undefined" !=
                typeof c.paths)
                for (pospaths = [], d = 0; d < c.paths.length; d++) {
                    pospaths[d] = eval("[" + c.paths[d].path + "]");
                    var h = L.polyline(eval("[" + c.paths[d].path + "]"), {
                        color: get_edit_color(d),
                        clickable: !1
                    });
                    b.extend(h.getBounds())
                }
            pos = b.getCenter();
            poszoom = map.getBoundsZoom(b);
            movingstep = 0;
            e && (searchformarker = e, clickmarker = g, deletemarker = L.marker(pos, {
                icon: icon_marker,
                title: "angegebene Position"
            }).addTo(map), added_marker());
            2 == map.getZoom() && window.setTimeout(mapgoto, 2E3);
            active_special = a;
            active_special_name = c.name;
            map.setView([0,
                0
            ], 2)
        }
    })
}

function get_edit_color(a) {
    var c = ["red", "blue", "green", "yellow"];
    return c[a % c.length]
}

function undo_edit() {
    if (0 < pHistory.length) {
        var a = pHistory.pop();
        1 == a && (pMarkerlayer.removeLayer(pMarker[pMarker.length - 1].marker), pMarker.pop());
        2 == a && (0 == paths[paths.length - 1].length && (paths.pop(), map.removeLayer(paths.length - 1)), paths[paths.length - 1].pop(), pLayer[paths.length - 1].clearLayers(), L.polyline(paths[paths.length - 1], {
            color: get_edit_color(paths.length - 1),
            clickable: !1
        }).addTo(pLayer[paths.length - 1]))
    }
}

function edit_path_add(a) {
    var c = paths.length - 1;
    paths[c].push(a);
    pLayer[c].clearLayers();
    L.polyline(paths[c], {
        color: get_edit_color(c),
        clickable: !1
    }).addTo(pLayer[c]);
    pHistory.push(2)
}

function encode_edit_settings() {
    for (var a = "", c = 0; c < pMarker.length; c++) a += "&lati" + c + "=" + encodeURI(pMarker[c].lati), a += "&longi" + c + "=" + encodeURI(pMarker[c].longi), a += "&name" + c + "=" + encodeURI(pMarker[c].name), a += "&description" + c + "=" + encodeURI(pMarker[c].description), a += "&web" + c + "=" + encodeURI(pMarker[c].web), a += "&image" + c + "=" + encodeURI(pMarker[c].image), a += "&video" + c + "=" + encodeURI(pMarker[c].video);
    a += "&marker=" + pMarker.length;
    for (c = 0; c < paths.length; c++) {
        for (var b = "", e = 0; e < paths[c].length; e++) var d = paths[c][e].toString(),
        d = d.replace(/\s+/g, ""), d = d.substr(7, d.length - 8), b = b + (",[" + d + "]");
        b = b.substr(1);
        a += "&path" + c + "=" + encodeURI(b)
    }
    return a += "&paths=" + paths.length
}

function save_edit() {
    var a = jQuery("#ename").val(),
        a = "&name=" + encodeURI(a) + "&cat1=" + jQuery("#ecat1").val() + "&cat2=" + jQuery("#ecat2").val(),
        a = a + encode_edit_settings();
    jQuery.post("ajax.php", "action=save-multi" + a, function (a) {
        if (a = eval("(" + a + ")")) "error" == a.result && notify_user(a.message), "ok" == a.result && (notify_user(a.message), close_edit_save(), abort_edit(), update_categories())
    })
}

function display_edit_save() {
    if (!multinameopen) {
        var a;
        a = '<h1>speichern</h1><p><label for="ename">Titel: </label><input type="text" id="ename" /></p><p><label for="ecat1">Kategorie: </label>';
        a += '<select id="ecat1">';
        for (var c = 0; c < categories.length; c++) 0 == categories[c].parent && (a += '<option value="' + categories[c].id + '">' + categories[c].name + "</option>");
        a += "</select></p>";
        a += '<p><label for="ecat2">Unterkategorie: </label>';
        a += '<select id="ecat2">';
        a += "</select></p>";
        a += '<p><input type="button" id="edit_save_button" value="speichern" /> <input type="button" id="edit_save_cancel" value="abbrechen" /></p>';
        jQuery("#multi-name").html(a);
        jQuery("#ecat1").change(refresh_edit_cat);
        jQuery("#edit_save_cancel").click(close_edit_save);
        jQuery("#edit_save_button").click(save_edit);
        refresh_edit_cat();
        jQuery("#multi-name").offset({
            top: -999
        }).show();
        window.setTimeout(open_edit_save, 100)
    }
}

function open_edit_save() {
    if (!multinameopen) {
        jQuery("#multi-name").show();
        var a = (jQuery(window).innerWidth() - jQuery("#multi-name").width()) / 2,
            c = jQuery("#multi-name").height() + 55,
            b = -10;
        multiopen && (b = jQuery("#multi-editor").offset().top + jQuery("#multi-editor").height() + 2);
        var e = b - 50,
            c = b - 50 - c;
        jQuery("#multi-name").offset({
            top: c,
            left: a
        }).animate({
            top: e
        }, 2E3);
        multinameopen = !0
    }
}

function close_edit_save() {
    if (multinameopen) {
        var a = -1 - (jQuery("#multi-name").height() + 55) - 10;
        jQuery("#multi-name").animate({
            top: a
        }, 2E3);
        multinameopen = !1
    }
}

function refresh_edit_cat() {
    for (var a = jQuery("#ecat1").val(), c = "", b = 0; b < categories.length; b++) categories[b].parent == a && (c += '<option value="' + categories[b].id + '">' + categories[b].name + "</option>");
    jQuery("#ecat2").html(c)
}

function start_multi() {
    multiopen || (map.closePopup(), close_edit_save(), jQuery("#multi-editor").animate({
        top: -58
    }, 2E3), selected_tool = 1, window.setTimeout("select_tool(1)", 1E3), paths = [
        []
    ], pLayer = [], pLayer[0] = new L.layerGroup, pLayer[0].addTo(map), pMarker = [], pMarkerlayer = new L.layerGroup, pMarkerlayer.addTo(map), pHistory = [], multiopen = !0)
}

function select_tool(a) {
    selected_tool = a;
    1 == a && (jQuery("#edit-set-wp").animate({
        "background-color": "#0b0"
    }, 2E3), jQuery("#edit-set-path").animate({
        "background-color": "#fff"
    }, 2E3));
    2 == a && (jQuery("#edit-set-wp").animate({
        "background-color": "#fff"
    }, 2E3), jQuery("#edit-set-path").animate({
        "background-color": "#0b0"
    }, 2E3), a = paths.length - 1, 0 < paths[a].length && (a++, paths[a] = [], pLayer[a] = new L.layerGroup, pLayer[a].addTo(map)))
}

function abort_edit() {
    multinameopen && close_edit_save();
    jQuery("#multi-editor").animate({
        top: -115
    }, 5E3);
    if ("undefined" != typeof pLayer)
        for (var a = 0; a < pLayer.length; a++) map.removeLayer(pLayer[a]);
    "undefined" != typeof pMarkerlayer && map.removeLayer(pMarkerlayer);
    delete pLayer;
    delete paths;
    delete pMarker;
    delete pHistory;
    map.closePopup();
    multiopen = !1
}

function load_marker() {
    if (1 == mapmode && !(5 > map.getZoom())) {
        var a = map.getBounds().toBBoxString();
        jQuery.post("ajax.php", {
            action: "getmarker",
            box: a,
            specialmarker: specialmarker
        }, function (a) {
            specialmarker = 0;
            if (a = eval("(" + a + ")")) {
                if (0 < a.result2)
                    for (var b = 0; b < a.sector.length; b++) {
                        var e = parseInt(a.sector[b].id);
                        0 > jQuery.inArray(e, loaded_sector) && (add_sectorname(a.sector[b].name, new L.LatLng(a.sector[b].lati, a.sector[b].longi)), loaded_sector.push(e))
                    }
                if (0 < a.result)
                    for (b = 0; b < a.marker.length; b++)
                        if (e = parseInt(a.marker[b].id),
                            0 > jQuery.inArray(e, loaded_marker) && (1 == a.marker[b].type && geosettings.wp || 2 == a.marker[b].type && geosettings.poi || 3 == a.marker[b].type && geosettings.vista || 4 == a.marker[b].type && geosettings.skillpoint || 5 == a.marker[b].type && geosettings.heart || 6 == a.marker[b].type && geosettings.jp || 7 == a.marker[b].type && geosettings.dungeon || 8 == a.marker[b].type && geosettings.explorator || 10 == a.marker[b].type)) {
                            loaded_marker.push(e);
                            var d;
                            1 == a.marker[b].type && (d = icon_wp);
                            2 == a.marker[b].type && (d = icon_poi);
                            3 == a.marker[b].type && (d =
                                icon_vista);
                            4 == a.marker[b].type && (d = icon_skillpoint);
                            5 == a.marker[b].type && (d = icon_heart);
                            6 == a.marker[b].type && (d = icon_jp);
                            7 == a.marker[b].type && (d = icon_dungeon);
                            8 == a.marker[b].type && (d = icon_explorator);
                            10 == a.marker[b].type && (d = icon_marker);
                            var g = L.marker([a.marker[b].lati, a.marker[b].longi], {
                                icon: d,
                                title: a.marker[b].name
                            });
                            1 == a.marker[b].type && layer_wp.addLayer(g);
                            2 == a.marker[b].type && layer_poi.addLayer(g);
                            3 == a.marker[b].type && layer_vista.addLayer(g);
                            4 == a.marker[b].type && layer_skillpoint.addLayer(g);
                            5 == a.marker[b].type && layer_heart.addLayer(g);
                            6 == a.marker[b].type && layer_jp.addLayer(g);
                            7 == a.marker[b].type && layer_dungeon.addLayer(g);
                            8 == a.marker[b].type && layer_explorator.addLayer(g);
                            10 == a.marker[b].type && ("undefined" != typeof aniMarker && aniMarker.clearLayers(), aniMarker = new L.layerGroup, aniMarker.addTo(map), aniMarker.addLayer(g));
                            if (4 != a.marker[b].type) jQuery(g).on("click", {
                                id: e,
                                type: a.marker[b].type,
                                marker: g
                            }, markerclick);
                            searchformarker == e && (embedded || markerclick({
                                data: {
                                    id: e,
                                    type: a.marker[b].type,
                                    marker: g
                                }
                            }), searchformarker = 0)
                        }
                jQuery(".leaflet-marker-icon").tooltip({
                    show: !1,
                    hide: !1,
                    content: function () {
                        if ("undefined" != typeof jQuery(this).attr("title")) {
                            var a = jQuery(this).attr("title").split("\n");
                            return '<span class="tooltip-title">' + a.shift() + "</span>" + (0 < a.length ? "<br />" + a.join("<br />") : "")
                        }
                    }
                });
                jQuery(".ui-tooltip").mouseover(function () {
                    jQuery(this).remove()
                })
            }
        })
    }
}

function markerclick(a) {
    (1 <= a.data.type && 8 >= a.data.type || 10 == a.data.type || 12 == a.data.type) && jQuery.post("ajax.php", {
        action: "getwp",
        id: a.data.id
    }, function (c) {
        (c = eval("(" + c + ")")) && ("undefined" == typeof c.marker ? notify_user("Marker nicht gefunden oder unzureichende Rechte.") : c.result && marker_display(c.marker.lati + "," + c.marker.longi, a.data.id, c.marker, a.data.marker, a.data.special, a.data.special_name))
    })
}

function prepare_marker_edit(a, c) {
    if (1 <= c && 8 >= c || 10 == c || 12 == c) map.closePopup(), jQuery.post("ajax.php", {
        action: "getwp",
        id: a
    }, function (b) {
        if ((b = eval("(" + b + ")")) && b.result) {
            var e = b.marker.lati + "," + b.marker.longi;
            1 == c && 1 == rights.addwp || 2 == c && 1 == rights.addpoi || 3 == c && 1 == rights.addvista || 4 == c && 1 == rights.addskillpoint || 5 == c && 1 == rights.addheart || 6 == c && 1 == rights.addjp || 7 == c && 1 == rights.adddungeon || 8 == c && 1 == rights.addexplorator || 10 == c && 1 == rights.single || 12 == c && 1 == rights.multi ? marker_mask({
                action: "mod",
                type: c,
                marker: lastmarker,
                id: a,
                pos: e,
                name: b.marker.name,
                descriptionsrc: b.marker.descriptionsrc,
                description: b.marker.description,
                web: b.marker.web,
                image: b.marker.image,
                video: b.marker.video
            }) : marker_display(e, a, b.marker, marker)
        }
    })
}

function marker_mask(a) {
    map.closePopup();
    var c = a.pos.split(","),
        b = "";
    1 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/waypoint32.png" />Wegmarke hinzuf\u00fcgen</h1>' : b + '<h1><img src="images/waypoint32.png" />Wegmarke bearbeiten</h1>');
    2 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/poi.png" />Sehensw\u00fcrdigkeit hinzuf\u00fcgen</h1>' : b + '<h1><img src="images/poi.png" />Sehensw\u00fcrdigkeit bearbeiten</h1>');
    3 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/vista.png" />Vista hinzuf\u00fcgen</h1>' :
        b + '<h1><img src="images/vista.png" />Vista bearbeiten</h1>');
    4 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/skillpoint.png" />Skillpoint hinzuf\u00fcgen</h1>' : b + '<h1><img src="images/skillpoint.png" />Skillpoint bearbeiten</h1>');
    5 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/heart.png" />Herz-Quest hinzuf\u00fcgen</h1>' : b + '<h1><img src="images/heart.png" />Herz-Quest bearbeiten</h1>');
    6 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/jp.png" />Sprungr\u00e4tsel hinzuf\u00fcgen</h1>' :
        b + '<h1><img src="images/jp.png" />Sprungr\u00e4tsel bearbeiten</h1>');
    7 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/dungeon.png" />Dungeon hinzuf\u00fcgen</h1>' : b + '<h1><img src="images/dungeon.png" />Dungeon bearbeiten</h1>');
    8 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/explorator-32.png" />Explorator-Marke hinzuf\u00fcgen</h1>' : b + '<h1><img src="images/explorator-32.png" />Explorator-Marke bearbeiten</h1>');
    10 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/little-flag.png" />Marker hinzuf\u00fcgen</h1>' :
        b + '<h1><img src="images/little-flag.png" />Marker bearbeiten</h1>');
    12 == a.type && (b = "add" == a.action ? b + '<h1><img src="images/little-flag.png" />Marker hinzuf\u00fcgen</h1>' : b + '<h1><img src="images/little-flag.png" />Marker bearbeiten</h1>');
    if (1 <= a.type && 8 >= a.type || 10 == a.type || 12 == a.type) {
        b += '<div style="width:680px"><div style="width:330px;float:left;">';
        b += '<p>Titel:<br /><input style="width:300px;" id="wpname" type="text" value="' + (a.name || "") + '" /></p>';
        b += '<p>Beschreibung:<br /><textarea style="width:300px;height:100px;" id="wpdescription">' +
            (a.descriptionsrc || "") + "</textarea></p>";
        b += "</div>";
        b += '<p>Link zu einer Webseite:<br /><input style="width:300px;" id="wpweb" type="text" value="' + (a.web || "") + '" /></p>';
        b += '<p>Link zu einem Bild:<br /><input style="width:300px;" id="wpimage" type="text" value="' + (a.image || "") + '" /></p>';
        b += '<p>Link zu einem Video:<br /><input style="width:300px;" id="wpvideo" type="text" value="' + (a.video || "") + '" /></p>';
        if (10 == a.type && "add" == a.action) {
            for (var b = b + '<p><label for="ecat1">Kategorie: </label>', b =
                    b + '<select id="ecat1">', e = 0; e < categories.length; e++) 0 == categories[e].parent && (b += '<option value="' + categories[e].id + '">' + categories[e].name + "</option>");
            b += "</select><br />";
            b += '<label for="ecat2">Unterkategorie: </label>';
            b += '<select id="ecat2">';
            b += "</select></p>"
        }
        b += '<p><input type="button" id="actionbutton" value="' + ("add" == a.action ? "hinzuf\u00fcgen" : "speichern") + '" /> <input id="deletebutton" type="button" value="l\u00f6schen" /> <input onclick="javascript:map.closePopup();" type="button" value="abbrechen" /></p>';
        b += "</div>";
        b += '<div style="clear:both;"></div>'
    }
    L.popup({
        maxWidth: 700
    }).setLatLng(c).setContent(b).openOn(map);
    b = c[0];
    c = c[1];
    if (1 <= a.type && 8 >= a.type)
        if (jQuery("#wpname").focus(), "add" == a.action) {
            var d = {
                lati: b,
                longi: c,
                type: a.type
            };
            jQuery("#actionbutton").on("click", d, marker_add);
            jQuery("#wpname").keydown(function (a) {
                13 == a.which && marker_add({
                    data: d
                })
            });
            jQuery("#deletebutton").hide()
        } else d = {
            marker: a.marker,
            id: a.id
        }, jQuery("#actionbutton").on("click", d, marker_mod), jQuery("#wpname").keydown(function (a) {
            13 ==
                a.which && marker_mod({
                    data: d
                })
        }), jQuery("#deletebutton").on("click", {
            id: a.id,
            marker: a.marker
        }, marker_del);
    10 == a.type && (jQuery("#ecat1").change(refresh_edit_cat), refresh_edit_cat(), jQuery("#wpname").focus(), "add" == a.action ? (d = {
        lati: b,
        longi: c,
        type: a.type
    }, jQuery("#actionbutton").on("click", d, single_marker_add), jQuery("#wpname").keydown(function (a) {
        13 == a.which && single_marker_add({
            data: d
        })
    })) : (d = {
        marker: a.marker,
        id: a.id
    }, jQuery("#actionbutton").on("click", d, marker_mod), jQuery("#wpname").keydown(function (a) {
        13 ==
            a.which && marker_mod({
                data: d
            })
    })), jQuery("#deletebutton").hide());
    12 == a.type && (jQuery("#wpname").focus(), "add" == a.action ? (d = {
        lati: b,
        longi: c,
        type: a.type
    }, jQuery("#actionbutton").on("click", d, marker_add_array), jQuery("#wpname").keydown(function (a) {
        13 == a.which && marker_add_array({
            data: d
        })
    }), jQuery("#deletebutton").hide()) : multiopen || (d = {
        marker: a.marker,
        id: a.id
    }, jQuery("#actionbutton").on("click", d, marker_mod), jQuery("#wpname").keydown(function (a) {
        13 == a.which && marker_mod({
            data: d
        })
    }), jQuery("#deletebutton").on("click", {
        id: a.id,
        marker: a.marker
    }, marker_del)))
}

function single_marker_add(a) {
    var c = jQuery("#wpname").val(),
        b = jQuery("#wpdescription").val(),
        e = jQuery("#wpweb").val(),
        d = jQuery("#wpimage").val(),
        g = jQuery("#wpvideo").val(),
        h = jQuery("#ecat1").val(),
        p = jQuery("#ecat2").val();
    jQuery.post("ajax.php", {
        action: "addsinglemarker",
        cat1: h,
        cat2: p,
        lati: a.data.lati,
        longi: a.data.longi,
        name: c,
        description: b,
        web: e,
        image: d,
        video: g,
        type: a.data.type
    }, function (a) {
        if (a = eval("(" + a + ")")) notify_user(a.message), "ok" == a.result && (map.closePopup(), update_categories())
    })
}

function common_marker_stuff(a, c, b, e) {
    var d = window.location.protocol + "//" + window.location.host + window.location.pathname + "?marker=" + a,
        g = window.location.protocol + "//" + window.location.host + window.location.pathname + "?special=" + b;
    a = '<div style="width:500px;padding:0;margin:0;"><div style="float:left;width:250px;padding:0;margin:0;">';
    a += '<p style="margin-top:0;"><strong>Diese Markierung als Link versenden:</strong><br />';
    a += '<input style="width:240px;" id="link1" type="text" value="' + d + '" /></p>';
    "undefined" !=
        typeof b && (a += "<p><strong>Special als Link versenden:</strong><br />", a += '<input style="width:240px;" id="link2" type="text" value="' + g + '" /></p>');
    a += "</div>";
    "" == c && (c = "Karte");
    a += "<p><strong>Markierung f\u00fcrs Forum:</strong><br />";
    a += '<input style="width:240px;" id="link3" type="text" value="[url=' + d + "]" + c + '[/url]" /></p>';
    "undefined" != typeof b && ("undefined" == typeof e && (e = "Karte"), a += "<p><strong>Special f\u00fcrs Forum:</strong><br />", a += '<input style="width:240px;" id="link4" type="text" value="[url=' +
        g + "]" + e + '[/url]" /></p>');
    return a += "</div>"
}

function marker_add_array(a) {
    var c = [];
    c.lati = a.data.lati;
    c.longi = a.data.longi;
    c.name = jQuery("#wpname").val();
    c.description = jQuery("#wpdescription").val();
    c.web = jQuery("#wpweb").val();
    c.image = jQuery("#wpimage").val();
    c.video = jQuery("#wpvideo").val();
    map.closePopup();
    m = L.marker([a.data.lati, a.data.longi], {
        icon: icon_marker,
        title: c.name
    });
    pMarkerlayer.addLayer(m);
    c.marker = m;
    pMarker.push(c);
    pHistory.push(1)
}

function marker_add(a) {
    var c = jQuery("#wpname").val(),
        b = jQuery("#wpdescription").val(),
        e = jQuery("#wpweb").val(),
        d = jQuery("#wpimage").val(),
        g = jQuery("#wpvideo").val();
    map.closePopup();
    jQuery.post("ajax.php", {
        action: "addmarker",
        lati: a.data.lati,
        longi: a.data.longi,
        name: c,
        description: b,
        web: e,
        image: d,
        video: g,
        type: a.data.type
    }, function (a) {
        if (a = eval("(" + a + ")")) notify_user(a.result), load_marker()
    })
}

function marker_mod(a) {
    var c = jQuery("#wpname").val(),
        b = jQuery("#wpdescription").val(),
        e = jQuery("#wpweb").val(),
        d = jQuery("#wpimage").val(),
        g = jQuery("#wpvideo").val();
    map.removeLayer(a.data.marker);
    loaded_marker.splice(jQuery.inArray(a.data.id, loaded_marker), 1);
    map.closePopup();
    jQuery.post("ajax.php", {
        action: "modmarker",
        name: c,
        id: a.data.id,
        description: b,
        web: e,
        image: d,
        video: g
    }, function (a) {
        if (a = eval("(" + a + ")")) notify_user(a.result), load_marker()
    })
}

function marker_del(a) {
    map.removeLayer(a.data.marker);
    map.closePopup();
    jQuery.post("ajax.php", {
        action: "delmarker",
        id: a.data.id
    }, function (a) {
        (a = eval("(" + a + ")")) && notify_user(a.result)
    })
}

function marker_display(a, c, b, e, d, g) {
    var h;
    h = "<h1>";
    lastmarker = e;
    if (1 == b.type && 1 == rights.addwp || 2 == b.type && 1 == rights.addpoi || 3 == b.type && 1 == rights.addvista || 4 == b.type && 1 == rights.addskillpoint || 5 == b.type && 1 == rights.addheart || 6 == b.type && 1 == rights.addjp || 7 == b.type && 1 == rights.adddungeon || 8 == b.type && 1 == rights.addexplorator || 10 == b.type && 1 == rights.single || 12 == b.type && 1 == rights.multi) h += '<a href="javascript:prepare_marker_edit(' + c + "," + b.type + ');"><img src="images/edit-16.png" /></a>';
    1 == b.type && (h += '<img src="images/waypoint32.png" />Wegmarke');
    2 == b.type && (h += '<img src="images/poi.png" />Sehensw\u00fcrdigkeit');
    3 == b.type && (h += '<img src="images/vista.png" />Vista');
    4 == b.type && (h += '<img src="images/skillpoint.png" />Skillpoint');
    5 == b.type && (h += '<img src="images/heart.png" />Herz-Quest');
    6 == b.type && (h += '<img src="images/jp.png" />Sprungr\u00e4tsel');
    7 == b.type && (h += '<img src="images/dungeon.png" />Dungeon');
    8 == b.type && (h += '<img src="images/explorator-32.png" />Explorator-Marke');
    10 == b.type && (h += '<img src="images/little-flag.png" />Markierung');
    12 == b.type && (h += '<img src="images/little-flag.png" />Markierung');
    b.name && (h += " \u00bb" + b.name + "\u00ab");
    h += "</h1>";
    b.description && (e = b.description.replace(/\n/g, "<br />"), h += '<div class="scrolltext">' + e + "</div>");
    if (b.web || b.image || b.video) {
        h += '<p class="morecontainer"><img src="images/moreinfo.png" width="23" height="49" style="margin-right: 15px;" />';
        e = decodeURI(b.image);
        e = e.replace(/'/g, "\\'");
        var p = b.name.replace(/'/g, "\\'");
        b.web && (h += '<a href="' + b.web + '" onclick="window.open(this.href);blur();return false;" title="Webseite' +
            (b.name ? " zu \u00bb" + b.name + "\u00ab" : "") + '"><img src="images/web.png" /></a>');
        b.image && (h += '<a href="' + b.image + '" title="Screenshot' + (b.name ? " zu \u00bb" + b.name + "\u00ab" : "") + '" onclick="Mediabox.open(\'' + e + "','Screenshot" + (b.name ? " zu \u00bb" + p + "\u00ab" : "") + '\');return false;"><img src="images/image.png" /></a>');
        b.video && (h += '<a href="' + clean_youtube_url(b.video) + '" title="Video' + (b.name ? " zu \u00bb" + b.name + "\u00ab" : "") + '" onclick="Mediabox.open(\'' + b.video + "','Video" + (b.name ? " zu \u00bb" + p + "\u00ab" :
            "") + '\');return false;"><img src="images/video.png" /></a>');
        h += "</p>";
        b.image && (h += copyright(b.image))
    }
    h += common_marker_stuff(c, b.name, d, g);
    a = a.split(",");
    L.popup({
        maxWidth: 600
    }).setLatLng(a).setContent(h).openOn(map);
    jQuery("#link1").click(function () {
        jQuery("#link1").select()
    });
    jQuery("#link2").click(function () {
        jQuery("#link2").select()
    });
    jQuery("#link3").click(function () {
        jQuery("#link3").select()
    });
    jQuery("#link4").click(function () {
        jQuery("#link4").select()
    })
}

function copyright(a) {
    var c = "";
    a.search(/:\/\/(.*?)\//) && (c = RegExp.$1.toLowerCase());
    return -1 < c.search(/wyld-bunch\.de/) || -1 < c.search(/imageshack\.us/) ? "" : '<div class="copyright">Copyright Bild &copy; by <a href="http://' + c + '" onclick="window.open(this.href);return false;">' + html_entities(c) + "</a></div>"
}

function clean_youtube_url(a) {
    c = a;
    if (a.match(/youtube\.com\/watch/i)) {
        var c = "http://youtube.com/watch?v=",
            b = 0;
        a = a.substr(a.indexOf("?") + 1).split("&");
        for (var e = 0; e < a.length; e++) {
            var d = a[e].split("=");
            "v" == d[0] && (c += d[1]);
            "start" == d[0] && (b = parseInt(d[1]))
        }
    }
    b && (c += "&t=" + b);
    return c
}

function add_zonename(a, c, b, e) {
    a = new(L.Class.extend({
        initialize: function (a) {
            this._latlng = a
        },
        onAdd: function (a) {
            this._map = a;
            this._el = L.DomUtil.create("div", "zonenname2 " + (e ? "" : "no") + "snow leaflet-zoom-hide");
            jQuery(this._el).html(c + '<p class="additional">' + b + "</p>");
            a.getPanes().overlayPane.appendChild(this._el);
            a.on("viewreset", this._reset, this);
            this._reset()
        },
        onRemove: function (a) {
            a.getPanes().overlayPane.removeChild(this._el);
            a.off("viewreset", this._reset, this)
        },
        _reset: function () {
            var a = this._map.latLngToLayerPoint(this._latlng);
            L.DomUtil.setPosition(this._el, a);
            L.DomUtil.removeClass(this._el, "zonenname7");
            L.DomUtil.removeClass(this._el, "zonenname6");
            L.DomUtil.removeClass(this._el, "zonenname5");
            L.DomUtil.removeClass(this._el, "zonenname4");
            L.DomUtil.removeClass(this._el, "zonenname3");
            L.DomUtil.removeClass(this._el, "zonenname2");
            a = map.getZoom();
            2 == mapmode && a++;
            L.DomUtil.addClass(this._el, "zonenname" + a)
        }
    }))(a);
    zonelist.push(a);
    map.addLayer(a)
}

function add_sectorname(a, c) {
    var b = new(L.Class.extend({
        initialize: function (a) {
            this._latlng = a
        },
        onAdd: function (b) {
            this._map = b;
            this._el = L.DomUtil.create("div", "sektorname2 darknosnow leaflet-zoom-hide");
            jQuery(this._el).html(a);
            b.getPanes().overlayPane.appendChild(this._el);
            b.on("viewreset", this._reset, this);
            this._reset()
        },
        onRemove: function (a) {
            a.getPanes().overlayPane.removeChild(this._el);
            a.off("viewreset", this._reset, this)
        },
        _reset: function () {
            var a = this._map.latLngToLayerPoint(this._latlng);
            L.DomUtil.setPosition(this._el,
                a);
            L.DomUtil.removeClass(this._el, "sektorname7");
            L.DomUtil.removeClass(this._el, "sektorname6");
            L.DomUtil.removeClass(this._el, "sektorname5");
            L.DomUtil.removeClass(this._el, "sektorname4");
            L.DomUtil.removeClass(this._el, "sektorname3");
            L.DomUtil.removeClass(this._el, "sektorname2");
            a = map.getZoom();
            2 == mapmode && (2 == a ? a = 4 : a++);
            7 < a && (a = 7);
            L.DomUtil.addClass(this._el, "sektorname" + a)
        }
    }))(c);
    sectorlist.push(b);
    map.addLayer(b)
}
jQuery(document).ready(function () {
    eventstate = [];
    eventgrace = [];
    eventlist = [];
    vorlesen = playSounds = !0;
    eventlist.push({
        type: 1,
        title: "Tequatl der Sonnenlose",
        target: [-52.60305, -6.50391],
        prequests: [],
        main: "568A30CF-8512-462F-9D67-647D69BEFAED",
        defend: "",
        sound_pre: "tequatl-pre",
        sound_main: "tequatl",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=y9i9FKf0agQ"
    });
    eventlist.push({
        type: 1,
        title: "Klaue von Jormag",
        target: [62.51232, 75.55298],
        prequests: ["429D6F3E-079C-4DE0-8F9D-8F75A222DB36", "96D736C4-D2C6-4392-982F-AC6B8EF3B1C8",
            "0CA3A7E3-5F66-4651-B0CB-C45D3F0CAD95", "C957AD99-25E1-4DB0-9938-F54D9F23587B", "BFD87D5B-6419-4637-AFC5-35357932AD2C"
        ],
        main: "0464CB9E-1848-4AAA-BA31-4779A959DD71",
        defend: "",
        sound_pre: "jormag-pre",
        sound_main: "jormag",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=S2LFxZuO77M"
    });
    eventlist.push({
        type: 1,
        title: "Der Zerschmetterer",
        target: [37.68382, 148.2605],
        prequests: ["580A44EE-BAED-429A-B8BE-907A18E36189"],
        main: "03BF176A-D59F-49CA-A311-39FC6F533F2F",
        defend: "",
        sound_pre: "zerschmetterer-pre",
        sound_main: "zerschmetterer",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=cKLOVzQWGCI"
    });
    eventlist.push({
        type: 1,
        title: "Der gefrorene Schlund",
        target: [34.8138, 76.43188],
        prequests: "6F516B2C-BD87-41A9-9197-A209538BB9DF D5F31E0B-E0E3-42E3-87EC-337B3037F437 6565EFD4-6E37-4C26-A3EA-F47B368C866D 90B241F5-9E59-46E8-B608-2507F8810E00 DB83ABB7-E5FE-4ACB-8916-9876B87D300D 374FC8CB-7AB7-4381-AC71-14BFB30D3019".split(" "),
        main: "F7D9D427-5E54-4F12-977A-9809B23FBA99",
        defend: "",
        sound_pre: "schlund-pre",
        sound_main: "schlund",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=OvXiuVmbucE"
    });
    eventlist.push({
        type: 1,
        title: "Golem Typ II",
        target: [-55.83214, 52.77832],
        prequests: ["3ED4FEB4-A976-4597-94E8-8BFD9053522F"],
        main: "",
        defend: "",
        sound_pre: "golem-pre",
        sound_main: "golem",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=34v8HIdL61M"
    });
    eventlist.push({
        type: 1,
        title: "Feuerelementar",
        target: [-12.45803, -95.35034],
        prequests: ["6B897FF9-4BA8-4EBD-9CEC-7DCFDA5361D8", "5E4E9CD9-DD7C-49DB-8392-C99E1EF4E7DF", "2C833C11-5CD5-4D96-A4CE-A74C04C9A278"],
        main: "33F76E9E-0BB6-46D0-A3A9-BE4CDFC4A3A4",
        defend: "",
        sound_pre: "feuerelementar-pre",
        sound_main: "feuerelementar",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=EYoPKNQepGA"
    });
    eventlist.push({
        type: 1,
        title: "Gro\u00dfer Dschungelwurm",
        target: [-4.20399, -74.34448],
        prequests: ["613A7660-8F3A-4897-8FAC-8747C12E42F8", "1DCFE4AA-A2BD-44AC-8655-BBD508C505D1", "61BA7299-6213-4569-948B-864100F35E16"],
        main: "C5972F64-B894-45B4-BC31-2DEEA6B7C033",
        defend: "",
        sound_pre: "wurm-pre",
        sound_main: "wurm",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=9uMkgFh4KOk"
    });
    eventlist.push({
        type: 1,
        title: "Schatten-Behemoth",
        target: [29.2864, -47.40601],
        prequests: ["CFBC4A8C-2917-478A-9063-1A8B43CC8C38", "E539A5E3-A33B-4D5F-AEED-197D2716F79B", "AFCF031A-F71D-4CEA-85E1-957179414B25", "36330140-7A61-4708-99EB-010B10420E39"],
        main: "31CEBA08-E44D-472F-81B0-7143D73797F5",
        defend: "",
        sound_pre: "behemoth-pre",
        sound_main: "behemoth",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=FQ6ZVgvcMCA"
    });
    eventlist.push({
        type: 1,
        title: "Feuer-Schamane",
        target: [59.37799, 121.1792],
        prequests: [],
        main: "295E8D3B-8823-4960-A627-23E07575ED96",
        defend: "",
        sound_pre: "feuerschamane-pre",
        sound_main: "feuerschamane",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=RZyqWHJprco"
    });
    eventlist.push({
        type: 1,
        title: "Megazerst\u00f6rer",
        target: [-58.64265, 30.60791],
        prequests: "294E08F6-CA36-42B3-8D06-B321BA06EECA 61D4579A-C53F-4C26-A31B-92FABE3DA566 3BA29A69-A30B-405D-96AC-CBA5D511C163 584A4D22-33DC-4D77-A5D9-2FA7379401ED 4210CE81-BDB7-448E-BE33-46E18A5A3477 9E5D9F1A-FE14-49C6-917F-43AAE227165C 36E81760-7D92-458E-AA22-7CDE94112B8F".split(" "),
        main: "C876757A-EF3E-4FBE-A484-07FF790D9B05",
        defend: "",
        sound_pre: "megadestroyer-pre",
        sound_main: "megadestroyer",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=ktijQlUoVdQ"
    });
    eventlist.push({
        type: 1,
        title: "Tore von Arah",
        target: [-77.19131, -50.27344],
        prequests: "80F7CC11-3116-42B5-A7C3-965EE5A69E51 9DA0E1E8-1A44-4A3C-9FCC-257350978CE9 6B5C8659-F3AF-4DFC-A6F5-CD6620E3BE11 7EA1BE90-C3CB-4598-A2DD-D56764785F7D E87A021D-4E7C-4A50-BEDB-6F5A54C90A9A B1B94EFD-4F67-4716-97C2-880CD16F1297".split(" "),
        main: "02DECBE6-A0BA-47CC-9256-A6D59881D92A",
        defend: "",
        sound_pre: "arah-pre",
        sound_main: "arah",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=XkegzFBzdn0"
    });
    eventlist.push({
        type: 1,
        title: "Karka-K\u00f6nigin",
        target: [-37.8228, -42.40723],
        prequests: [],
        main: "E1CC6E63-EFFE-4986-A321-95C89EA58C07",
        defend: "",
        sound_pre: "karka-pre",
        sound_main: "karka",
        sound_success: "",
        video: ""
    });
    eventlist.push({
        type: 1,
        title: "Auge des Zhaitan",
        target: [-68.22867, -13.55713],
        prequests: "77ADD8A0-7A1F-47D7-8D82-484A2134F576 88DBFB0B-3CB5-47FE-8B9D-2A9E3E04F7F2 9841ACE7-5F49-45FF-AE56-16B6908A09FE A0E5E563-2701-4D4E-8163-A89FEA02EC38 6FA8BE3F-9F6C-4790-BFBC-380B26FDB06C 42884028-C274-4DFA-A493-E750B8E1B353".split(" "),
        main: "A0796EC5-191D-4389-9C09-E48829D1FDB2",
        defend: "",
        sound_pre: "auge-pre",
        sound_main: "auge",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=Bd_tlBuCT8Q"
    });
    eventlist.push({
        type: 1,
        title: "Schaufler-Kommissar",
        target: [-15.02969, 37.81494],
        prequests: ["10F7854A-D2E7-42E4-9BCE-0C411EF91328", "141654A6-D42E-415E-A3C4-918A1E664AF3"],
        main: "95CA969B-0CC6-4604-B166-DBCCE125864F",
        defend: "",
        sound_pre: "kommissar-pre",
        sound_main: "kommissar",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=aSTskqEC0sY"
    });
    eventlist.push({
        type: 1,
        title: "Faulb\u00e4r-H\u00e4uptling",
        target: [-19.89072, 157.21436],
        prequests: ["D9F1CF48-B1CB-49F5-BFAF-4CEC5E68C9CF", "4B478454-8CD2-4B44-808C-A35918FA86AA"],
        main: "B4E6588F-232C-4F68-9D58-8803D67E564D",
        defend: "",
        sound_pre: "faul-pre",
        sound_main: "faul",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=TdJERBb3-lw"
    });
    eventlist.push({
        type: 1,
        title: "Ulgoth der Modniir",
        target: [58.63122, -0.95581],
        prequests: "4A1DECF3-C1AD-42EC-9905-976B281CFA49 AE7AAA0C-5619-4C94-918B-6022DB9AA481 38E99B3A-C4E5-4F1E-B5B9-24DD3DF5EDB4 C3A1BAE2-E7F2-4929-A3AA-92D39283722C DDC0A526-A239-4791-8984-E7396525B648 DA465AE1-4D89-4972-AD66-A9BE3C5A1823 A3101CDC-A4A0-4726-85C0-147EF8463A50".split(" "),
        main: "E6872A86-E434-4FC1-B803-89921FF0F6D6",
        defend: "",
        sound_pre: "ulgoth-pre",
        sound_main: "ulgoth",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=H4UlMqndWCY"
    });
    eventlist.push({
        type: 1,
        title: "Admiral Covington",
        target: [-7.79808, -4.48242],
        prequests: ["B6B7EE2A-AD6E-451B-9FE5-D5B0AD125BB2", "189E7ABE-1413-4F47-858E-4612D40BF711", "0E0801AF-28CF-4FF7-8064-BB2F4A816D23"],
        main: "242BD241-E360-48F1-A8D9-57180E146789",
        defend: "",
        sound_pre: "covington-pre",
        sound_main: "covington",
        sound_success: "",
        video: "http://www.youtube.com/watch?v=zhXwf7t6CPE"
    });
    eventlist.push({
        type: 2,
        title: "Tempel des Grenth",
        target: [-79.17959, -65.92896],
        prequests: ["C8139970-BE46-419B-B026-485A14002D44", "E16113B1-CE68-45BB-9C24-91523A663BCB"],
        main: "99254BA6-F5AE-4B07-91F1-61A9E7C51A51",
        defend: "57A8E394-092D-4877-90A5-C238E882C320",
        sound_pre: "grenth-pre",
        sound_main: "grenth",
        sound_success: "grenth-suc",
        video: "http://www.youtube.com/watch?v=s4taY_NsJEA"
    });
    eventlist.push({
        type: 2,
        title: "Tempel der Lyssa",
        target: [-63.45051, -36.12305],
        prequests: ["F66922B5-B4BD-461F-8EC5-03327BD2B558",
            "35997B10-179B-4E39-AD7F-54E131ECDD57", "590364E0-0053-4933-945E-21D396B10B20"
        ],
        main: "0372874E-59B7-4A8F-B535-2CF57B8E67E4",
        defend: "BFA71CD0-ED1E-4EE3-B8A9-B23B98C3B786",
        sound_pre: "lyssa-pre",
        sound_main: "lyssa",
        sound_success: "lyssa-suc",
        video: "http://www.youtube.com/watch?v=y6G0nUV54Ao"
    });
    eventlist.push({
        type: 2,
        title: "Tempel des Balthasar",
        target: [-67.20829, -13.89771],
        prequests: ["D3FFC041-4124-4AA7-A74B-B9363ED1BCBD", "A8D1A2B7-1F1B-413D-8E64-06CA0D26712D", "45B84A62-BE33-4371-B9FB-CC8490528276", "D0ECDACE-41F8-46BD-BB17-8762EF29868C",
            "7B7D6D27-67A0-44EF-85EA-7460FFA621A1"
        ],
        main: "2555EFCB-2927-4589-AB61-1957D9CC70C8",
        defend: "589B1C41-DD96-4AEE-8A3A-4CC607805B05",
        sound_pre: "balthasar-pre",
        sound_main: "balthasar",
        sound_success: "balthasar-suc",
        video: "http://www.youtube.com/watch?v=vuwKCegoRPY"
    });
    eventlist.push({
        type: 2,
        title: "Tempel der Dwayna",
        target: [-65.18764, -47.97729],
        prequests: ["F531683F-FC09-467F-9661-6741E8382E24", "7EF31D63-DB2A-4FEB-A6C6-478F382BFBCB", "526732A0-E7F2-4E7E-84C9-7CDED1962000"],
        main: "6A6FD312-E75C-4ABF-8EA1-7AE31E469ABA",
        defend: "0723E056-E665-439F-99B7-20385442AD4E",
        sound_pre: "dwayna-pre",
        sound_main: "dwayna",
        sound_success: "dwayna-suc",
        video: "http://www.youtube.com/watch?v=BPKWRfrlHvM"
    });
    eventlist.push({
        type: 2,
        title: "Tempel der Melandru",
        target: [-74.25974, -55.87646],
        prequests: ["3D333172-24CE-47BA-8F1A-1AD47E7B69E4", "E7563D8D-838D-4AF4-80CD-1D3A25B6F6AB", "F0CE1E71-4B96-48C6-809D-E1941AF40B1D", "351F7480-2B1C-4846-B03B-ED1B8556F3D7", "7E24F244-52AF-49D8-A1D7-8A1EE18265E0"],
        main: "A5B5C2AF-22B1-4619-884D-F231A0EE0877",
        defend: "04902E61-A102-4D32-860D-C14B150BD4F5",
        sound_pre: "melandru-pre",
        sound_main: "melandru",
        sound_success: "melandru-suc",
        video: "http://www.youtube.com/watch?v=_3Xbt7599XI"
    });
    events[0] = jQuery("#event-suche").prop("checked");
    events[1] = jQuery("#event-kiste").prop("checked");
    events[2] = jQuery("#event-tempel").prop("checked");
    events[3] = jQuery("#event-tempel-open").prop("checked");
    events[4] = jQuery("#event-simple").prop("checked");
    events[5] = jQuery("#event-group").prop("checked");
    check_event_timer_state();
    speaklist = [];
    jQuery("#sounds").append('<audio id="speak" preload="none" ></audio>');
    jQuery("#speak").on("ended", do_speak);
    speaking = !1;
    jQuery("#speak").on("playing", function () {
        speaking = !0
    });
    display_speaking_error = !0
});

function start_speak() {
    speaking || do_speak()
}

function do_speak() {
    var a = "";
    speaking ? 0 < speaklist.length && (a = speaklist.shift()) : a = "pre";
    "" != a ? (jQuery("#speak").html('<source id="oggspeak" src="events/' + a + '.ogg" type="audio/ogg"/><source id="mp3speak" src="events/' + a + '.mp3" type="audio/mpeg"/>'), document.getElementById("speak").load(), document.getElementById("speak").play()) : speaking = !1
}

function generate_serverlist(a) {
    if (a = eval(a)) {
        "undefined" == typeof server && (server = get_cookie("server"), "" == server && (server = 2203));
        if (0 == server || 2E3 > server || 2999 < server) server = 2203;
        for (var c = !1, b = 0; b < a.length; b++) a[b].id == server && (c = !0);
        c || (notify_user("Server-ID " + server + " ung\u00fcltig."), server = 2203);
        a.sort(function (a, b) {
            return a.name < b.name ? -1 : a.name == b.name ? 0 : 1
        });
        for (b = 0; b < a.length; b++) 2E3 <= a[b].id && 2999 >= a[b].id && (a[b].id == server ? (jQuery("<option />").val(a[b].id).text(a[b].name).attr("selected", !0).appendTo("#world"), jQuery("<option />").val(a[b].id).text(a[b].name).attr("selected", !0).appendTo("#world2")) : (jQuery("<option />").val(a[b].id).text(a[b].name).appendTo("#world"), jQuery("<option />").val(a[b].id).text(a[b].name).appendTo("#world2")));
        loadevents && (loadevents = !1, check_event_timer_state());
        serverdataloaded = !0;
        jQuery("#world").change(copyvalues);
        jQuery("#world2").change(copyvalues)
    }
}

function copyvalues() {
    var a = jQuery(this).val();
    jQuery("#world").val(a);
    jQuery("#world2").val(a);
    set_cookie("server", a);
    jQuery("#layer-nodes").prop("checked") && add_nodes()
}

function event_layer(a) {
    if (angemeldet || !forced_login) {
        a = !1;
        for (var c = 0; 5 >= c; c++) events[c] && (a = !0);
        special_active && (a = !0);
        special_active = !1;
        "event-kiste" == jQuery(this).attr("id") && (events[1] = jQuery("#event-kiste").prop("checked"));
        "event-tempel" == jQuery(this).attr("id") && (events[2] = jQuery("#event-tempel").prop("checked"), events[2] && (jQuery("#event-tempel-open").removeAttr("checked"), events[3] = 0));
        "event-tempel-open" == jQuery(this).attr("id") && (events[3] = jQuery("#event-tempel-open").prop("checked"),
            events[3] && (jQuery("#event-tempel").removeAttr("checked"), events[2] = 0));
        "event-suche" == jQuery(this).attr("id") && (events[0] = jQuery("#event-suche").prop("checked"));
        if ("event-simple" == jQuery(this).attr("id") && (events[4] = jQuery("#event-simple").prop("checked"), events[4]))
            for (jQuery("#event-group").removeAttr("checked"), c = events[5] = 0; c < special_events.length; c++) jQuery("#" + special_events[c].dom_id).removeAttr("checked");
        "event-group" == jQuery(this).attr("id") && (events[5] = jQuery("#event-group").prop("checked"),
            events[5] && (jQuery("#event-simple").removeAttr("checked"), events[4] = 0));
        for (c = 0; c < special_events.length; c++) jQuery(this).prop("checked") && (special_active = !0), jQuery(this).attr("id") == special_events[c].dom_id && jQuery(this).prop("checked") && (jQuery("#event-simple").removeAttr("checked"), events[4] = 0);
        "event-alarm" == jQuery(this).attr("id") && (jQuery("#event-alarm").prop("checked") ? (playSounds = !0, vorlesen = !1, jQuery("#event-speak").removeAttr("checked")) : playSounds = !1);
        "event-speak" == jQuery(this).attr("id") &&
            (jQuery("#event-speak").prop("checked") ? (vorlesen = playSounds = !0, jQuery("#event-alarm").removeAttr("checked")) : playSounds = vorlesen = !1);
        "event-alarm" == jQuery(this).attr("id") && (playSounds = jQuery(this).prop("checked"));
        for (var b = !1, c = 0; 5 >= c; c++) events[c] && (b = !0);
        special_active && (b = !0);
        a && !b && remove_noapi();
        (!b || !a && b) && check_event_timer_state()
    }
}

function check_event_timer_state() {
    for (var a = !1, c = 0; 5 >= c; c++) events[c] && (a = !0);
    special_active && (a = !0);
    !angemeldet && forced_login && (a = !1);
    a ? update_events() : (window.clearTimeout(event_timer), layer_prequests.clearLayers(), layer_events.clearLayers())
}

function update_events() {
    window.clearTimeout(event_timer);
    if (angemeldet || !forced_login) {
        var a = "F9FC4D88-0409-4F1E-8189-E189AC6B1A9D 7BA6EEA3-3613-4CA6-BE54-C1C7EC7E5C80 5380DB84-B81E-4533-B6FF-ECB52B3AC15B C2F3B0D8-0B30-4EE8-BEE0-344FD0055BBC C0D53DBF-B31B-4284-B9BD-69F637B32640 04B33C34-8B6D-482A-B918-A00EE8D5B259 C42BDB41-AEB6-4BD7-8451-373D3FA15A1D 1C39087B-488D-4ABF-B21E-DB48B0FD4EEA EB5EF94C-C52C-4887-A459-1819858EDF56 D3FFC041-4124-4AA7-A74B-B9363ED1BCBD 101CEAA6-0A5E-4D7D-B185-BE324B88190C D06C6415-07AC-4893-9361-F214742B5241 11E3A32E-F955-4C9D-99B0-5B53279ADDC9 E5BF70E5-7219-479D-9C65-650E85745DDA 51CF2675-4242-4CF1-B14F-70F70A674257 2428CB21-0CA5-413B-9342-DD4C2BEC9865 7C5AE370-5EBC-4C05-A29B-6A4F4093B466 C7078D50-A3CE-4A3C-859E-A351F38B5547 A8D1A2B7-1F1B-413D-8E64-06CA0D26712D 11473024-4349-4443-8D88-C6494E33693B D1A02D5B-20A4-4923-A7A1-91852FE786E8 4F125457-A110-4860-8D76-E4AE678C3023 9A3A5E8E-C9BF-4867-94D3-82336702B010 0B4C8548-B366-4033-BE89-E4A22F625957 BA43EBDF-9276-4155-8A7B-03A58D621211 DBA880A3-7EBD-4575-936C-16DBC3376AF2 7FC9708B-F219-4CC0-94DB-594B30DF697C 29E4724B-846C-4C58-AF6A-0752B447602F 9713F90A-728A-4062-9674-41B8FB960E14 45B84A62-BE33-4371-B9FB-CC8490528276".split(" "),
            c =
                jQuery("#world").val();
        jQuery.ajax({
            type: "GET",
            url: "https://api.guildwars2.com/v1/events.json?world_id=" + c,
            timeout: 15E3
        }).done(function (b) {
            if (b = eval(b)) {
                try {
                    jQuery(".leaflet-marker-icon").tooltip("close")
                } catch (e) {}
                remove_noapi();
                layer_prequests.clearLayers();
                layer_events.clearLayers();
                var d = 0,
                    g = 0,
                    h = 0;
                loaded_event_details || jQuery.get("https://api.guildwars2.com/v1/event_details.json?lang=de&world_id=" + c, function (a) {
                    (event_details = eval(a)) || (loaded_event_details = !1);
                    loaded_event_details = !0
                });
                if (events[0] ||
                    events[1] || events[2] || events[4] || events[5] || special_active || "undefined" != typeof a)
                    if (loaded_map_details || (notify_user("Lade Event-Details..."), jQuery.get("https://api.guildwars2.com/v1/maps.json?lang=de", function (a) {
                        (map_details = eval(a)) || (loaded_map_details = !1);
                        loaded_map_details = !0
                    })), loaded_map_details && loaded_event_details)
                        for (var p = jQuery("#event-such-text").val().toLowerCase(), f = 0; f < b.events.length; f++)
                            if ("Active" == b.events[f].state && "undefined" != typeof event_details.events[b.events[f].event_id]) {
                                for (var q =
                                    0 <= jQuery.inArray("group_event", event_details.events[b.events[f].event_id].flags), t = -1, l = !1, n = 0; n < special_events.length; n++)
                                    if (jQuery("#" + special_events[n].dom_id).prop("checked") || events[4]) {
                                        for (var k = 0; k < special_events[n].keywords.length; k++) - 1 != event_details.events[b.events[f].event_id].name.toLowerCase().indexOf(special_events[n].keywords[k].toLowerCase()) && (t = n);
                                        for (k = 0; k < special_events[n].event_ids.length; k++) b.events[f].event_id == special_events[n].event_ids[k] && (t = n)
                                    }
                                for (n = 0; n < eventlist.length; n++)
                                    for (k =
                                        0; k < eventlist[n].prequests.length; k++)
                                        if (1 == eventlist[n].type && events[1] || 2 == eventlist[n].type && events[2])
                                            if (eventlist[n].prequests[k] == b.events[f].event_id && (l = !0), 2 == eventlist[n].type && events[2])
                                                for (var r = 0; r < a.length; r++) a[r] == b.events[f].event_id && (L.polyline([
                                                    [-57.33245, -1.61499],
                                                    [-57.79209, -3.51562],
                                                    [-58.33257, -4.96582],
                                                    [-58.88194, -6.29517],
                                                    [-59.00663, -7.43774],
                                                    [-60.365, -9.63501],
                                                    [-63.64138, -10.55786]
                                                ], {
                                                    color: "red",
                                                    clickable: !1
                                                }).addTo(layer_events), L.polyline([
                                                    [-61.04233, 7.67944],
                                                    [-61.11141,
                                                        6.02051
                                                    ],
                                                    [-61.35988, 0.29663],
                                                    [-62.51739, -3.52661],
                                                    [-63.25836, -7.14111],
                                                    [-63.76307, -10.15137]
                                                ], {
                                                    color: "red",
                                                    clickable: !1
                                                }).addTo(layer_events), L.polyline([
                                                    [-62.17063, 8.33862],
                                                    [-64.37319, 8.78906],
                                                    [-65.63109, 4.97681],
                                                    [-65.15535, 4.48242],
                                                    [-65.41259, -1.07666],
                                                    [-65.73966, -1.92261],
                                                    [-67.13583, -5.33936],
                                                    [-63.92772, -10.55786]
                                                ], {
                                                    color: "red",
                                                    clickable: !1
                                                }).addTo(layer_prequests), l = !0);
                                if (events[0]) var s = -1 != event_details.events[b.events[f].event_id].name.toLowerCase().indexOf(p);
                                if (events[0] && s && 0 < p.length ||
                                    events[4] || events[5] && q || special_active && 0 <= t || l)
                                    if (n = [event_details.events[b.events[f].event_id].location.center[0], event_details.events[b.events[f].event_id].location.center[1]], "undefined" != typeof map_details.maps[event_details.events[b.events[f].event_id].map_id]) {
                                        k = map_details.maps[event_details.events[b.events[f].event_id].map_id].map_rect;
                                        r = map_details.maps[event_details.events[b.events[f].event_id].map_id].continent_rect;
                                        percentageX = (n[0] - k[0][0]) / (k[1][0] - k[0][0]);
                                        percentageY = 1 - (n[1] - k[0][1]) /
                                            (k[1][1] - k[0][1]);
                                        continentX = r[0][0] + (r[1][0] - r[0][0]) * percentageX;
                                        continentY = r[0][1] + (r[1][1] - r[0][1]) * percentageY;
                                        var n = [continentX, continentY],
                                            r = layer_events,
                                            x = 0,
                                            k = icon_event_simple;
                                        q && (k = icon_event_group);
                                        0 <= t && (k = special_events[t].icon);
                                        l && (k = icon_dragon_part, r = layer_prequests, x = -1E4);
                                        events[0] && 0 < p.length && s && (k = icon_event_suche);
                                        L.marker(map.unproject(n, map.getMaxZoom()), {
                                            icon: k,
                                            riseOnHover: !0,
                                            zIndexOffset: x,
                                            riseOffset: 2E4,
                                            title: event_details.events[b.events[f].event_id].name + " (" + event_details.events[b.events[f].event_id].level +
                                                ")"
                                        }).addTo(r)
                                    }
                            }
                for (f = 0; f < eventlist.length; f++)
                    if (events[eventlist[f].type] || events[3] && 2 == eventlist[f].type) {
                        "undefined" == typeof eventstate[f] && (eventstate[f] = 0, eventgrace[f] = [0, 0]);
                        s = eventlist[f].prequests.length;
                        p = 0;
                        q = "";
                        for (t = 0; t < s; t++)
                            for (l = 0; l < b.events.length; l++) b.events[l].event_id == eventlist[f].prequests[t] && "Active" == b.events[l].state && (p = t + 1);
                        for (l = 0; l < b.events.length; l++) b.events[l].event_id == eventlist[f].main && (q = b.events[l].state);
                        if (2 == eventlist[f].type)
                            for (l = 0; l < b.events.length; l++) b.events[l].event_id ==
                                eventlist[f].defend && "Warmup" == b.events[l].state && (q = "open");
                        t = 0;
                        0 < eventgrace[f][0] && (eventgrace[f][0] -= 15, p < eventgrace[f][1] && (t = 1, p = eventgrace[f][1]), 0 >= eventgrace[f][0] && 3 == eventstate[f] && (eventstate[f] = 4));
                        if (1 == eventlist[f].type)
                            if (0 == p && ("Active" != q && "Preparation" != q) && (eventstate[f] = 0, eventgrace[f] = [0, 0]), (0 < p || "Preparation" == q) && "Active" != q) {
                                if (t || (eventgrace[f] = [240, p]), 1 != eventstate[f] && (d = eventstate[f] = 1, vorlesen && "" != eventlist[f].sound_pre && speaklist.push(eventlist[f].sound_pre), 1 < s ? notify_user("Pre-Events zu \u00bb" +
                                        eventlist[f].title + "\u00ab haben begonnen...") : notify_user("Das Event \u00bb" + eventlist[f].title + "\u00ab kann ausgel\u00f6st werden...")), 1 < s ? (q = p + "/" + s + " Pre-Events aktiv", loaded_event_details && "undefined" != typeof event_details.events[eventlist[f].prequests[p - 1]] && (q += "\n\u00bb" + event_details.events[eventlist[f].prequests[p - 1]].name + "\u00ab")) : q = "kann aktiviert werden/in Vorbereitung", s = 10 * Math.floor(10 * p / (s + 1)), 90 < s && (s = 90), k = "", k = 10 > s ? icon_dragon_prepare : eval("icon_dragon" + s), "" != eventlist[f].video &&
                                    (q += "\nAnclicken f\u00fcr Videoanleitung"), showmarker = L.marker(eventlist[f].target, {
                                        icon: k,
                                        title: eventlist[f].title + "\n" + q
                                    }).addTo(layer_events), "" != eventlist[f].video) showmarker.on("click", function (a, b) {
                                    return function () {
                                        Mediabox.open(a, b)
                                    }
                                }(eventlist[f].video, eventlist[f].title))
                            } else {
                                if ("Active" == q && (2 != eventstate[f] && (eventstate[f] = 2, eventgrace[f] = [0, 0], g = 1, vorlesen && "" != eventlist[f].sound_main && speaklist.push(eventlist[f].sound_main), notify_user("Das Event \u00bb" + eventlist[f].title + "\u00ab hat begonnen!")),
                                    q = "Event l\u00e4uft", "" != eventlist[f].video && (q += "\nAnclicken f\u00fcr Videoanleitung"), showmarker = L.marker(eventlist[f].target, {
                                        icon: icon_dragon_active,
                                        title: eventlist[f].title + "\n" + q
                                    }).addTo(layer_events), "" != eventlist[f].video)) showmarker.on("click", function (a, b) {
                                    return function () {
                                        Mediabox.open(a, b)
                                    }
                                }(eventlist[f].video, eventlist[f].title))
                            } else if (2 == eventlist[f].type)
                            if ((0 < p || "Preparation" == q) && "Active" != q && "open" != q && !events[3]) {
                                if (t || (eventgrace[f] = [240, p]), 1 != eventstate[f] && (d = eventstate[f] =
                                        1, vorlesen && "" != eventlist[f].sound_pre && speaklist.push(eventlist[f].sound_pre), 1 <= s ? notify_user("Pre-Events zu \u00bb" + eventlist[f].title + "\u00ab haben begonnen...") : notify_user("Das Event \u00bb" + eventlist[f].title + "\u00ab kann ausgel\u00f6st weden...")), 1 < s ? (q = p + "/" + s + " Pre-Events aktiv", loaded_event_details && "undefined" != typeof event_details.events[eventlist[f].prequests[p - 1]] && (q += "\n\u00bb" + event_details.events[eventlist[f].prequests[p - 1]].name + "\u00ab")) : q = "kann aktiviert werden/in Vorbereitung",
                                    "" != eventlist[f].video && (q += "\nAnclicken f\u00fcr Videoanleitung"), showmarker = L.marker(eventlist[f].target, {
                                        icon: icon_temple_prepare,
                                        title: eventlist[f].title + "\n" + q
                                    }).addTo(layer_events), "" != eventlist[f].video) showmarker.on("click", function (a, b) {
                                    return function () {
                                        Mediabox.open(a, b)
                                    }
                                }(eventlist[f].video, eventlist[f].title))
                            } else if ("Active" == q) {
                            if (2 != eventstate[f] && (eventstate[f] = 2, eventgrace[f] = [0, 0], g = 1, vorlesen && "" != eventlist[f].sound_main && speaklist.push(eventlist[f].sound_main), notify_user("Das Haupt-Event zum \u00bb" +
                                eventlist[f].title + "\u00ab l\u00e4uft...")), q = "Event l\u00e4uft", "" != eventlist[f].video && (q += "\nAnclicken f\u00fcr Videoanleitung"), showmarker = L.marker(eventlist[f].target, {
                                icon: icon_temple_last,
                                title: eventlist[f].title + "\n" + q
                            }).addTo(layer_events), "" != eventlist[f].video) showmarker.on("click", function (a, b) {
                                return function () {
                                    Mediabox.open(a, b)
                                }
                            }(eventlist[f].video, eventlist[f].title))
                        } else if ("open" == q) {
                            if (2 == eventstate[f] || 0 == eventstate[f]) 2 == eventstate[f] && (h = 1, vorlesen && "" != eventlist[f].sound_success &&
                                speaklist.push(eventlist[f].sound_success), notify_user("Der \u00bb" + eventlist[f].title + "\u00ab wurde erfolgreich abgeschlossen!")), eventstate[f] = 3;
                            if (3 == eventstate[f] && (q = "Tempel ge\u00f6ffnet", "" != eventlist[f].video && (q += "\nAnclicken f\u00fcr Videoanleitung"), showmarker = L.marker(eventlist[f].target, {
                                icon: icon_temple_open,
                                title: eventlist[f].title + "\n" + q
                            }).addTo(layer_events), "" != eventlist[f].video)) showmarker.on("click", function (a, b) {
                                return function () {
                                    Mediabox.open(a, b)
                                }
                            }(eventlist[f].video, eventlist[f].title))
                        }
                    }
                playSounds &&
                    (vorlesen && (h || g || d) ? start_speak() : h ? document.getElementById("opened").play() : g ? document.getElementById("started").play() : d && document.getElementById("pre").play());
                jQuery(".leaflet-marker-icon").tooltip({
                    show: !1,
                    hide: !1,
                    content: function () {
                        if ("undefined" != typeof jQuery(this).attr("title")) {
                            var a = jQuery(this).attr("title").split("\n");
                            return '<span class="tooltip-title">' + a.shift() + "</span>" + (0 < a.length ? "<br />" + a.join("<br />") : "")
                        }
                    }
                });
                jQuery(".ui-tooltip").mouseover(function () {
                    jQuery(this).remove()
                })
            } else noapi()
        }).fail(function () {
            noapi()
        });
        event_timer = window.setTimeout(update_events, 15E3)
    }
}
jQuery(document).ready(function () {
    wvwo = [{
        id: 1,
        name: "Aussichtspunkt",
        type: "f",
        position: [-75.97888, 56.55762]
    }, {
        id: 2,
        name: "Tal",
        type: "f",
        position: [-81.97857, 72.61963]
    }, {
        id: 3,
        name: "Tiefland",
        type: "f",
        position: [-82.00611, 31.09131]
    }, {
        id: 4,
        name: "Golanta-Lichtung",
        type: "l",
        position: [-82.91598, 43.5498]
    }, {
        id: 5,
        name: "Pangloss-Anh\u00f6he",
        type: "l",
        position: [-76.39331, 66.88477]
    }, {
        id: 6,
        name: "Speldan Kahlschlag",
        type: "l",
        position: [-75.45307, 36.71631]
    }, {
        id: 7,
        name: "Danelon-Passage",
        type: "l",
        position: [-83.22348, 62.90771]
    }, {
        id: 8,
        name: "Umberlichtung Forst",
        type: "l",
        position: [-79.58043, 74.50928]
    }, {
        id: 9,
        name: "Schloss Steinnebel",
        type: "s",
        position: [-80.14868, 53.04199]
    }, {
        id: 10,
        name: "Schurkenbruch",
        type: "l",
        position: [-79.53264, 30.52002]
    }, {
        id: 11,
        name: "Aldons Vorsprung",
        type: "t",
        position: [-80.90762, 26.89453]
    }, {
        id: 12,
        name: "Wildbachstrecke",
        type: "t",
        position: [-80.2831, 37.74902]
    }, {
        id: 13,
        name: "Jerrifers Sumpfloch",
        type: "t",
        position: [-82.80676, 35.37598]
    }, {
        id: 14,
        name: "Klovan-Senke",
        type: "t",
        position: [-81.8581, 43.52783]
    }, {
        id: 15,
        name: "Langor-Schlucht",
        type: "t",
        position: [-83.02622, 71.65283]
    }, {
        id: 16,
        name: "Quentinsee",
        type: "t",
        position: [-82.28238, 58.53516]
    }, {
        id: 17,
        name: "Mendons Spalt",
        type: "t",
        position: [-75.19141, 45.37354]
    }, {
        id: 18,
        name: "Anzalias-Pass",
        type: "t",
        position: [-78.08469, 44.01123]
    }, {
        id: 19,
        name: "Ogerwacht-Kanal",
        type: "t",
        position: [-76.9752, 61.98486]
    }, {
        id: 20,
        name: "Veloka-Hang",
        type: "t",
        position: [-75.05602, 63.74268]
    }, {
        id: 21,
        name: "Durios-Schlucht",
        type: "t",
        position: [-79.93592, 65.21484]
    }, {
        id: 22,
        name: "Bravost-Abhang",
        type: "t",
        position: [-80.91456,
            78.64014
        ]
    }, {
        id: 23,
        name: "blaue Festung",
        type: "f",
        position: [-67.76771, 128.8916]
    }, {
        id: 24,
        name: "Dom\u00e4ne des Meisters",
        type: "l",
        position: [-77.88343, 129.39697]
    }, {
        id: 25,
        name: "Rotstrauch",
        type: "t",
        position: [-74.20002, 122.146]
    }, {
        id: 26,
        name: "Gr\u00fcnsee",
        type: "t",
        position: [-74.60761, 140.44922]
    }, {
        id: 27,
        name: "Bucht des Aufstiegs",
        type: "f",
        position: [-71.72577, 106.3916]
    }, {
        id: 28,
        name: "Horst der Morgend\u00e4mmerung",
        type: "t",
        position: [-64.20638, 142.69043]
    }, {
        id: 29,
        name: "Der Geisterholm",
        type: "l",
        position: [-55.36662,
            129.6167
        ]
    }, {
        id: 30,
        name: "Wald-Freistatt",
        type: "t",
        position: [-64.60504, 115.46631]
    }, {
        id: 31,
        name: "Askalion-H\u00fcgel",
        type: "f",
        position: [-71.20192, 155.14893]
    }, {
        id: 32,
        name: "Etheron-H\u00fcgel",
        type: "f",
        position: [-51.84935, 76.46484]
    }, {
        id: 33,
        name: "Traumbucht",
        type: "f",
        position: [-52.9089, 27.72949]
    }, {
        id: 34,
        name: "H\u00fctte des Siegers",
        type: "l",
        position: [-64.98865, 50.71289]
    }, {
        id: 35,
        name: "Gr\u00fcnstrauch",
        type: "t",
        position: [-57.68066, 43.43994]
    }, {
        id: 36,
        name: "Blausee",
        type: "t",
        position: [-58.50517, 61.74316]
    }, {
        id: 37,
        name: "rote Festung",
        type: "f",
        position: [-45.29035, 50.20752]
    }, {
        id: 38,
        name: "Weitsicht",
        type: "t",
        position: [-39.60569, 36.71631]
    }, {
        id: 39,
        name: "Das Gottschwert",
        type: "l",
        position: [-23.80545, 50.84473]
    }, {
        id: 40,
        name: "Felswand",
        type: "t",
        position: [-38.78835, 63.9624]
    }, {
        id: 41,
        name: "Shadaran-H\u00fcgel",
        type: "f",
        position: [-75.23627, -2.3291]
    }, {
        id: 42,
        name: "Rotsee",
        type: "t",
        position: [-77.93865, -17.09473]
    }, {
        id: 43,
        name: "H\u00fctte des Helden",
        type: "l",
        position: [-80.4886, -28.10303]
    }, {
        id: 44,
        name: "Schreckensfall-Bucht",
        type: "f",
        position: [-75.64498, -51.08643]
    }, {
        id: 45,
        name: "Blaustrauch",
        type: "t",
        position: [-77.59413, -35.31006]
    }, {
        id: 46,
        name: "gr\u00fcne Festung",
        type: "f",
        position: [-72.49511, -28.58643]
    }, {
        id: 47,
        name: "Sonnenh\u00fcgel",
        type: "t",
        position: [-70.0356, -42.07764]
    }, {
        id: 48,
        name: "Glaubenssprung",
        type: "l",
        position: [-70.97403, -47.19727]
    }, {
        id: 49,
        name: "Blautal-Zuflucht",
        type: "l",
        position: [-78.13449, -45.81299]
    }, {
        id: 50,
        name: "Blauwasser-Tiefland",
        type: "l",
        position: [-59.45624, 71.34521]
    }, {
        id: 51,
        name: "Astralholm",
        type: "l",
        position: [-40.89691, 71.74072]
    }, {
        id: 52,
        name: "Arahs Hoffnung",
        type: "l",
        position: [-41.88592, 31.35498]
    }, {
        id: 53,
        name: "Gr\u00fcntal-Zuflucht",
        type: "l",
        position: [-58.98399, 32.69531]
    }, {
        id: 54,
        name: "Nebel-Freistatt",
        type: "l",
        position: [-70.74348, -6.72363]
    }, {
        id: 55,
        name: "Rotwasser-Tiefland",
        type: "l",
        position: [-78.31831, -7.55859]
    }, {
        id: 56,
        name: "Die Titanenpranke",
        type: "l",
        position: [-62.56298, -27.90527]
    }, {
        id: 57,
        name: "Felsenspitze",
        type: "t",
        position: [-69.67236, -14.80957]
    }, {
        id: 58,
        name: "G\u00f6tterkunde",
        type: "l",
        position: [-65.77574,
            110.28076
        ]
    }, {
        id: 59,
        name: "Rottal-Zuflucht",
        type: "l",
        position: [-74.87362, 111.5332]
    }, {
        id: 60,
        name: "Sternenhain",
        type: "l",
        position: [-65.58572, 150.86426]
    }, {
        id: 61,
        name: "Gr\u00fcnwasser-Tiefland",
        type: "l",
        position: [-75.10128, 150.20508]
    }, {
        id: 62,
        name: "Tempel der verlorenen Gebete",
        type: "1",
        position: [-57.46859, 50.44922]
    }, {
        id: 63,
        name: "Schlachten-Senke",
        type: "2",
        position: [-54.53383, 43.83545]
    }, {
        id: 64,
        name: "Bauers Anwesen",
        type: "3",
        position: [-49.69606, 46.25244]
    }, {
        id: 65,
        name: "Obstgarten-Aussichtspunkt",
        type: "4",
        position: [-50.20503, 56.79932]
    }, {
        id: 66,
        name: "Aufstieg des Schnitzers",
        type: "5",
        position: [-54.55932, 57.10693]
    }, {
        id: 71,
        name: "Tempel der verlorenen Gebete",
        type: "1",
        position: [-74.11605, 129.15527]
    }, {
        id: 70,
        name: "Schlachten-Senke",
        type: "2",
        position: [-72.61369, 122.51953]
    }, {
        id: 69,
        name: "Bauers Anwesen",
        type: "3",
        position: [-70.05809, 125.00244]
    }, {
        id: 68,
        name: "Obstgarten-Aussichtspunkt",
        type: "4",
        position: [-70.31134, 135.68115]
    }, {
        id: 67,
        name: "Aufstieg des Schnitzers",
        type: "5",
        position: [-72.63993, 135.94482]
    }, {
        id: 76,
        name: "Tempel der verlorenen Gebete",
        type: "1",
        position: [-77.5421, -28.32275]
    }, {
        id: 75,
        name: "Schlachten-Senke",
        type: "2",
        position: [-76.36226, -34.93652]
    }, {
        id: 74,
        name: "Bauers Anwesen",
        type: "3",
        position: [-74.32517, -32.49756]
    }, {
        id: 73,
        name: "Obstgarten-Aussichtspunkt",
        type: "4",
        position: [-74.55504, -22.0166]
    }, {
        id: 72,
        name: "Aufstieg des Schnitzers",
        type: "5",
        position: [-76.32595, -21.64307]
    }];
    icon_w1b = L.icon({
        iconUrl: "images/wvw/638721.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w1r = L.icon({
        iconUrl: "images/wvw/638724.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w1g = L.icon({
        iconUrl: "images/wvw/638723.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w1n = L.icon({
        iconUrl: "images/wvw/638725.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w2b = L.icon({
        iconUrl: "images/wvw/638706.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w2r = L.icon({
        iconUrl: "images/wvw/638708.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w2g = L.icon({
        iconUrl: "images/wvw/638707.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w2n = L.icon({
        iconUrl: "images/wvw/638709.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w3b = L.icon({
        iconUrl: "images/wvw/638710.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w3r = L.icon({
        iconUrl: "images/wvw/638712.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w3g = L.icon({
        iconUrl: "images/wvw/638711.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w3n = L.icon({
        iconUrl: "images/wvw/638714.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w4b = L.icon({
        iconUrl: "images/wvw/638718.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w4r = L.icon({
        iconUrl: "images/wvw/638720.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w4g = L.icon({
        iconUrl: "images/wvw/638719.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w4n = L.icon({
        iconUrl: "images/wvw/638722.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w5b = L.icon({
        iconUrl: "images/wvw/638713.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w5r = L.icon({
        iconUrl: "images/wvw/638716.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w5g = L.icon({
        iconUrl: "images/wvw/638715.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_w5n = L.icon({
        iconUrl: "images/wvw/638717.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wlr = L.icon({
        iconUrl: "images/wvw/157379.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wtr = L.icon({
        iconUrl: "images/wvw/157404.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wfr = L.icon({
        iconUrl: "images/wvw/157396.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wsr = L.icon({
        iconUrl: "images/wvw/157383.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wlg = L.icon({
        iconUrl: "images/wvw/157378.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wtg = L.icon({
        iconUrl: "images/wvw/157403.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wfg = L.icon({
        iconUrl: "images/wvw/157395.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wsg = L.icon({
        iconUrl: "images/wvw/157382.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wlb = L.icon({
        iconUrl: "images/wvw/157377.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wtb = L.icon({
        iconUrl: "images/wvw/157402.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wfb = L.icon({
        iconUrl: "images/wvw/157394.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    });
    icon_wsb = L.icon({
        iconUrl: "images/wvw/157381.png",
        iconSize: [32, 32],
        iconAnchor: [16, 17]
    })
});

function toggle_tiles(a) {
    guildcache = [];
    "undefined" != typeof tiles && map.removeLayer(tiles);
    if (1 == a) mapmode = 1, remove_noapi(), wvwstatusopen && toggle_wvwstatus(), remove_all_countdowns(), "undefined" != typeof wvwtimer && window.clearTimeout(wvwtimer), layer_prequests.clearLayers(), layer_events.clearLayers(), jQuery("#layer-geo").show().prev("h3").show(), forced_login || (jQuery("#layer-event").show().prev("h3").show(), jQuery("#layer-search").show().prev("h3").show(), jQuery(".category").show().prev().show()), jQuery("#menu-accordion").accordion("refresh"),
    jQuery("#praecat").accordion("refresh"), update_cat_menu(), "undefined" != typeof countdown_active && (window.clearInterval(countdown_active), countdown_active = void 0), jQuery("#map").animate({
        backgroundColor: "#000"
    }, 2E3), tiles = L.tileLayer("tiles/{z}-{x}-{y}.jpg", {
        attribution: 'Kartendaten &copy; <a href="http://www.guildwars2.com">ArenaNet</a>, <a href="http://www.wyld-bunch.de/datenschutzerklaerung/">Datenschutz</a>',
        maxZoom: 7,
        minZoom: 2,
        continuousWorld: !1,
        noWrap: !0
    }).addTo(map), deactivate_map(), window.setTimeout(function () {
        map.setView([0,
            0
        ], 2);
        activate_map()
    }, 5E3), display_zones();
    else if (2 == a) {
        menuopen && togglemenu();
        old_owner = [];
        limitarea = !1;
        jQuery("#event-suche").attr("checked", !1);
        jQuery("#event-kiste").attr("checked", !1);
        jQuery("#event-tempel").attr("checked", !1);
        jQuery("#event-tempel-open").attr("checked", !1);
        jQuery("#event-simple").attr("checked", !1);
        jQuery("#event-group").attr("checked", !1);
        jQuery("#event-duell").attr("checked", !1);
        jQuery("#event-duell2").attr("checked", !1);
        jQuery("#layer-nodes").attr("checked", !1);
        layer_nodes.clearLayers();
        for (a = 0; 5 >= a; a++) events[a] = !1;
        for (a = 0; a < special_events.length; a++) jQuery("#" + special_events[a].dom_id).attr("checked", !1), special_events = !1;
        window.clearTimeout(event_timer);
        layer_prequests.clearLayers();
        layer_events.clearLayers();
        show_remove_marker = !1;
        jQuery("#remove_marker").fadeOut().delay(350).remove();
        "undefined" != typeof layer_delpath && layer_delpath.clearLayers();
        "undefined" != typeof aniPaths && aniPaths.clearLayers();
        "undefined" != typeof aniMarker && aniMarker.clearLayers();
        jQuery("#layer-geo").hide().prev("h3").hide();
        jQuery("#layer-event").hide().prev("h3").hide();
        jQuery("#layer-search").hide().prev("h3").hide();
        jQuery(".category").hide().prev().hide();
        jQuery("#menu-accordion").accordion("refresh");
        jQuery("#praecat").accordion("refresh");
        deactivate_map();
        mapmode = 2;
        jQuery("#map").animate({
            backgroundColor: "#fff"
        }, 2E3);
        tiles = L.tileLayer("https://tiles.guildwars2.com/2/1/{z}/{x}/{y}.jpg", {
            attribution: 'Kartendaten &copy; <a href="http://www.guildwars2.com">ArenaNet</a>, <a href="http://www.wyld-bunch.de/datenschutzerklaerung/">Datenschutz</a>',
            maxZoom: 6,
            minZoom: 2,
            continuousWorld: !1,
            noWrap: !0
        }).addTo(map);
        window.setTimeout(function () {
            var a = get_cookie("wvw_zoom"),
                b = get_cookie("wvw_pos"),
                e = get_cookie("savepos");
            "" != a && "" != b && 1 == e ? map.setView(eval("new L." + b), a) : map.setView([-70.49557, 53.61328], 3);
            activate_map()
        }, 5E3);
        "undefined" == typeof wvwzones ? load_wvw_zones() : display_wvw_zones();
        update_wvw()
    }
}

function clean_nodes() {
    for (; 0 < zonelist.length;) {
        var a = zonelist.pop();
        map.removeLayer(a)
    }
    for (; 0 < sectorlist.length;) a = sectorlist.pop(), map.removeLayer(a);
    loaded_sector = []
}

function remove_all_countdowns() {
    for (var a = countdownlist.length - 1; 0 <= a; a--) "undefined" != typeof countdownlist[a] && (map.removeLayer(countdownlist[a][1]), countdownlist[a] = void 0);
    for (a = taglist.length - 1; 0 <= a; a--) "undefined" != typeof taglist[a] && (map.removeLayer(taglist[a][1]), taglist[a] = void 0);
    prognostiziert = !1
}

function load_wvw_zones() {
    clean_nodes();
    jQuery.get("https://api.guildwars2.com/v1/map_floor.json?continent_id=2&floor=1&lang=de", function (a) {
        wvwzones = [];
        wvwsectors = [];
        a = eval(a);
        for (var c in a.regions)
            for (var b in a.regions[c].maps) {
                var e = [map.unproject(a.regions[c].maps[b].continent_rect[0], map.getMaxZoom()), map.unproject(a.regions[c].maps[b].continent_rect[1], map.getMaxZoom())];
                wvwzones.push([a.regions[c].maps[b].name, a.regions[c].name, 0, e, b, a.regions[c].maps[b].name]);
                for (e = 0; e < a.regions[c].maps[b].sectors.length; e++) wvwsectors.push([a.regions[c].maps[b].sectors[e].name,
                    map.unproject(a.regions[c].maps[b].sectors[e].coord, map.getMaxZoom()), a.regions[c].maps[b].sectors[e].sector_id, a.regions[c].maps[b].sectors[e].name
                ])
            }
        display_wvw_zones()
    })
}

function display_wvw_zones() {
    clean_nodes();
    for (var a = 0; a < wvwzones.length; a++) {
        var c = new L.LatLngBounds(wvwzones[a][3]);
        add_zonename(c.getCenter(), wvwzones[a][0], wvwzones[a][1], wvwzones[a][2])
    }
    for (a = 0; a < wvwsectors.length; a++) add_sectorname(wvwsectors[a][0], wvwsectors[a][1])
}

function display_zones() {
    clean_nodes();
    for (var a = 0; a < zones.length; a++) {
        var c = new L.LatLngBounds(zones[a][3]);
        add_zonename(c.getCenter(), zones[a][0], zones[a][1], zones[a][2])
    }
}

function load_match() {
    loaded_match = !1;
    serverdataloaded && "undefined" != typeof wvwzones ? (addlog("Lade Matches...", 1E4, !1), remove_all_countdowns(), ruina = [], kugeln = ["", "", ""], jQuery.get("https://api.guildwars2.com/v1/wvw/matches.json", function (a) {
        a = eval(a);
        old_owner = [];
        for (var c = jQuery("#world").val(), b = 0; b < a.wvw_matches.length; b++) {
            var e = !1;
            if (a.wvw_matches[b].red_world_id == c || a.wvw_matches[b].blue_world_id == c || a.wvw_matches[b].green_world_id == c) {
                e = !0;
                a.wvw_matches[b].red_world_id == c && (myworld = "red");
                a.wvw_matches[b].blue_world_id == c && (myworld = "blue");
                a.wvw_matches[b].green_world_id == c && (myworld = "green");
                match_id = a.wvw_matches[b].wvw_match_id;
                red = jQuery('#world [value="' + a.wvw_matches[b].red_world_id + '"]').text();
                blue = jQuery('#world [value="' + a.wvw_matches[b].blue_world_id + '"]').text();
                green = jQuery('#world [value="' + a.wvw_matches[b].green_world_id + '"]').text();
                jQuery("#redteam").html(html_entities(red) + '<div class="imagebox"></div>');
                jQuery("#blueteam").html(html_entities(blue) + '<div class="imagebox"></div>');
                jQuery("#greenteam").html(html_entities(green) + '<div class="imagebox"></div>');
                jQuery("#greenbar").css({
                    width: "4%"
                });
                jQuery("#bluebar").css({
                    width: "4%"
                });
                jQuery("#redbar").css({
                    width: "4%"
                });
                jQuery("#greenbar2").css({
                    width: "4%"
                });
                jQuery("#bluebar2").css({
                    width: "4%"
                });
                jQuery("#redbar2").css({
                    width: "4%"
                });
                for (var d = 0; d < wvwzones.length; d++) 96 == wvwzones[d][4] && (wvwzones[d][0] = blue, wvwzones[d][1] = "blaues Grenzland"), 95 == wvwzones[d][4] && (wvwzones[d][0] = green, wvwzones[d][1] = "gr\u00fcnes Grenzland"), 94 == wvwzones[d][4] &&
                    (wvwzones[d][0] = red, wvwzones[d][1] = "rotes Grenzland");
                for (d = 0; d < wvwsectors.length; d++) 834 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + blue), 836 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + blue), 843 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + red), 845 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + red), 848 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + green), 850 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + green), 941 == wvwsectors[d][2] && (wvwsectors[d][0] =
                    wvwsectors[d][3] + red), 952 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + red), 966 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + blue), 967 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + green), 974 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + green), 977 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + red), 979 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + blue), 980 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + blue), 992 == wvwsectors[d][2] && (wvwsectors[d][0] =
                    wvwsectors[d][3] + green), 993 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + green), 997 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + red), 1E3 == wvwsectors[d][2] && (wvwsectors[d][0] = wvwsectors[d][3] + blue);
                display_wvw_zones()
            }
            e && (loaded_match = !0);
            wvwstatusopen || toggle_wvwstatus()
        }
    })) : addlog("Lade Serverdaten...", 1E4, !1)
}

function toggle_wvwstatus() {
    wvwstatusopen ? (jQuery("#wvwstatus").animate({
        left: -420
    }, 3E3), wvwstatusopen = !1) : (jQuery("#wvwstatus").animate({
        left: -100
    }, 3E3), wvwstatusopen = !0)
}

function update_wvw() {
    var a = 0,
        c = 0,
        b = 0,
        e, d = jQuery("#world").val();
    d == oldserver && loaded_match ? (wvwstatusopen || toggle_wvwstatus(), jQuery.ajax({
        type: "GET",
        url: "https://api.guildwars2.com/v1/wvw/match_details.json?match_id=" + match_id,
        timeout: 5E3
    }).done(function (d) {
        if (d = eval(d)) {
            remove_noapi();
            try {
                jQuery(".leaflet-marker-icon").tooltip("close")
            } catch (h) {}
            layer_events.clearLayers();
            for (var p = jQuery("#ruins").prop("checked"), f = "", q = "", t = "", l = 0; l < d.maps.length; l++) {
                if ("undefined" != typeof d.maps[l].bonuses)
                    for (var n =
                        0; n < d.maps[l].bonuses.length; n++)
                        if ("bloodlust" == d.maps[l].bonuses[n].type) {
                            var k = "";
                            "BlueHome" == d.maps[l].type && (k = "b");
                            "RedHome" == d.maps[l].type && (k = "r");
                            "GreenHome" == d.maps[l].type && (k = "g");
                            "Blue" == d.maps[l].bonuses[n].owner && (f += k);
                            "Red" == d.maps[l].bonuses[n].owner && (q += k);
                            "Green" == d.maps[l].bonuses[n].owner && (t += k)
                        }
                for (n = 0; n < d.maps[l].objectives.length; n++)
                    for (k = 0; k < wvwo.length; k++)
                        if (wvwo[k].id == d.maps[l].objectives[n].id) {
                            e = "w" + wvwo[k].type + d.maps[l].objectives[n].owner.charAt(0).toLowerCase();
                            var r = 0;
                            switch (wvwo[k].type) {
                            case "l":
                                r = 5;
                                break;
                            case "t":
                                r = 10;
                                break;
                            case "f":
                                r = 25;
                                break;
                            case "s":
                                r = 35
                            }
                            switch (d.maps[l].objectives[n].owner) {
                            case "Red":
                                a += r;
                                break;
                            case "Blue":
                                b += r;
                                break;
                            case "Green":
                                c += r
                            }
                            e = eval("icon_" + e);
                            r = "";
                            "undefined" != typeof d.maps[l].objectives[n].owner_guild && ("undefined" != typeof guildcache[d.maps[l].objectives[n].owner_guild] ? (r = "\n" + guildcache[d.maps[l].objectives[n].owner_guild][0], r += "\nHash: " + d.maps[l].objectives[n].owner_guild) : r = "\nvon einer Gilde beansprucht, ich sage Dir gleich, wie sie hei\u00dft :-)");
                            showmarker = L.marker(wvwo[k].position, {
                                icon: e,
                                title: wvwo[k].name + r
                            }).addTo(layer_events);
                            if ("undefined" != typeof old_owner[wvwo[k].id]) {
                                if (old_owner[wvwo[k].id][0] != d.maps[l].objectives[n].owner) {
                                    var s, r = "<span>";
                                    "Green" == d.maps[l].objectives[n].owner && (s = '<span class="green">' + html_entities(green) + "</span>");
                                    "Red" == d.maps[l].objectives[n].owner && (s = '<span class="red">' + html_entities(red) + "</span>");
                                    "Blue" == d.maps[l].objectives[n].owner && (s = '<span class="blue">' + html_entities(blue) + "</span>");
                                    "Green" ==
                                        old_owner[wvwo[k].id][0] && (r = '<span class="green">');
                                    "Red" == old_owner[wvwo[k].id][0] && (r = '<span class="red">');
                                    "Blue" == old_owner[wvwo[k].id][0] && (r = '<span class="blue">');
                                    "Neutral" == d.maps[l].objectives[n].owner ? remove_countdown(wvwo[k].id) : add_countdown(wvwo[k].position, wvwo[k].id);
                                    if ((61 >= wvwo[k].id || p) && (!1 == limitarea || limitarea.contains(wvwo[k].position))) {
                                        var x = '<img src="' + e.options.iconUrl + '" width="16" height="16" /> ',
                                            w = "";
                                        62 <= wvwo[k].id && !1 == limitarea && (w = red, 67 <= wvwo[k].id && (w = blue), 72 <=
                                            wvwo[k].id && (w = green), w = " auf " + w);
                                        "Neutral" != d.maps[l].objectives[n].owner && addlog(x + r + html_entities(wvwo[k].name) + w + "</span> geh\u00f6rt nun " + s + ".")
                                    }
                                    old_owner[wvwo[k].id] = [d.maps[l].objectives[n].owner, ""];
                                    jQuery(showmarker._icon).css({
                                        width: 100,
                                        height: 100
                                    }).animate({
                                        width: 32,
                                        height: 32
                                    }, 5E3);
                                    if ("undefined" != typeof taglist)
                                        for (r = 0; r < taglist.length; r++) "undefined" != typeof taglist[r] && taglist[r][0] == wvwo[k].id && (map.removeLayer(taglist[r][1]), taglist[r] = void 0)
                                }
                                if ("undefined" != typeof d.maps[l].objectives[n].owner_guild) d.maps[l].objectives[n].owner_guild !=
                                    old_owner[wvwo[k].id][1] && (r = "<span>", "Green" == old_owner[wvwo[k].id][0] && (r = '<span class="green">'), "Red" == old_owner[wvwo[k].id][0] && (r = '<span class="red">'), "Blue" == old_owner[wvwo[k].id][0] && (r = '<span class="blue">'), (!1 == limitarea || limitarea.contains(wvwo[k].position)) && guildlog(r + html_entities(wvwo[k].name) + "</span> wurde von der Gilde " + r + "[GUILD]</span> beansprucht.", d.maps[l].objectives[n].owner_guild, wvwo[k].position, wvwo[k].id), old_owner[wvwo[k].id][1] = d.maps[l].objectives[n].owner_guild);
                                else if ("undefined" != typeof old_owner[wvwo[k].id] && "undefined" != typeof taglist)
                                    for (r = 0; r < taglist.length; r++) "undefined" != typeof taglist[r] && taglist[r][0] == wvwo[k].id && (map.removeLayer(taglist[r][1]), taglist[r] = void 0)
                            } else old_owner[wvwo[k].id] = [d.maps[l].objectives[n].owner, ""], "undefined" != typeof d.maps[l].objectives[n].owner_guild && (old_owner[wvwo[k].id][1] = d.maps[l].objectives[n].owner_guild, silentguildtag(d.maps[l].objectives[n].owner_guild, wvwo[k].position, wvwo[k].id))
                        }
            }
            s = "";
            for (p = 0; p < f.length; p++) s +=
                '<img src="images/wvw/kugel' + f.charAt(p) + '.png" title="Buffs" />';
            jQuery("#blueteam .imagebox").html(s);
            s = "";
            for (p = 0; p < q.length; p++) s += '<img src="images/wvw/kugel' + q.charAt(p) + '.png" title="Buffs" />';
            jQuery("#redteam .imagebox").html(s);
            s = "";
            for (p = 0; p < t.length; p++) s += '<img src="images/wvw/kugel' + t.charAt(p) + '.png" title="Buffs" />';
            jQuery("#greenteam .imagebox").html(s);
            jQuery(".leaflet-marker-icon").tooltip({
                show: !1,
                hide: !1,
                content: function () {
                    if ("undefined" != typeof jQuery(this).attr("title")) {
                        for (var a =
                            jQuery(this).attr("title").split("\n"), b = "", c = 0; c < a.length; c++) "Hash:" == a[c].substr(0, 5) && (b = a[c].substr(6), a[c] = void 0);
                        a = '<span class="tooltip-title">' + a.shift() + "</span>" + (0 < a.length ? "<br />" + a.join("<br />") : "");
                        "" != b && (a += '<div id="tooltiptag" style="width: 128px; height: 128px; overflow: hidden;"></div>', window.setTimeout(function () {
                            gw2emblem.init("tooltiptag", 128, "transparent");
                            gw2emblem.drawEmblemGw2(guildcache[b][2])
                        }, 10));
                        return a
                    }
                }
            });
            jQuery(".ui-tooltip").mouseover(function () {
                jQuery(this).remove()
            });
            f = a;
            b > f && (f = b);
            c > f && (f = c);
            s = 96 * a / f + 4 + "%";
            p = 96 * b / f + 4 + "%";
            l = 96 * c / f + 4 + "%";
            jQuery("#greenbar").css({
                width: l
            });
            jQuery("#bluebar").css({
                width: p
            });
            jQuery("#redbar").css({
                width: s
            });
            slidevalues(jQuery("#greenbar").next(".barvalue"), parseInt(jQuery("#greenbar").next(".barvalue").text()), c, 1);
            slidevalues(jQuery("#bluebar").next(".barvalue"), parseInt(jQuery("#bluebar").next(".barvalue").text()), b, 1);
            slidevalues(jQuery("#redbar").next(".barvalue"), parseInt(jQuery("#redbar").next(".barvalue").text()), a, 1);
            q = d.scores[0];
            t = d.scores[1];
            d = d.scores[2];
            f = q;
            t > f && (f = t);
            d > f && (f = d);
            s = 96 * q / f + 4 + "%";
            p = 96 * t / f + 4 + "%";
            l = 96 * d / f + 4 + "%";
            jQuery("#greenbar2").css({
                width: l
            });
            jQuery("#bluebar2").css({
                width: p
            });
            jQuery("#redbar2").css({
                width: s
            });
            slidevalues(jQuery("#greenbar2").next(".barvalue"), parseInt(jQuery("#greenbar2").next(".barvalue").text()), d, 1);
            slidevalues(jQuery("#bluebar2").next(".barvalue"), parseInt(jQuery("#bluebar2").next(".barvalue").text()), t, 1);
            slidevalues(jQuery("#redbar2").next(".barvalue"), parseInt(jQuery("#redbar2").next(".barvalue").text()),
                q, 1);
            if (!prognostiziert) {
                var u, v;
                "green" == myworld && (u = d, v = green);
                "blue" == myworld && (u = t, v = blue);
                "red" == myworld && (u = q, v = red);
                s = new Date;
                p = new Date;
                n = l = s.getDay();
                5 >= l && (n = 5 - l);
                0 == n && 20 <= s.getHours() && (n = 7);
                p.setTime(s.getTime() + 864E5 * n);
                p.setHours(20);
                p.setMinutes(0);
                p.setSeconds(0);
                p.setMilliseconds(0);
                s = Math.floor((p.getTime() - s.getTime()) / 9E5);
                u == f ? (u = 0, d != f && d > u && (u = d), t != f && t > u && (u = t), q != f && q > u && (u = q), d = v + " kann nicht mehr \u00fcberholt werden!", u + 695 * s > f && (d = v + " kann theoretisch noch \u00fcberholt werden, ist aber eher unwahrscheinlich!"),
                    u + 395 * s > f && (d = v + " kann noch \u00fcberholt werden!")) : (d = v + " hat keine Chance mehr, zu gewinnen!", u + 695 * s > f && (d = v + " hat noch eine eher theoretische M\u00f6glichkeit, zu gewinnen!"), u + 395 * s > f && (d = v + " hat noch eine realistische Chance, zu gewinnen!"));
                addlog(html_entities(d), void 0, !1);
                prognostiziert = !0
            }
        } else noapi()
    }).fail(function () {
        noapi()
    })) : (load_match(d), oldserver = d);
    2 == mapmode && (wvwtimer = window.setTimeout(update_wvw, 5E3))
}

function slidevalues(a, c, b, e) {
    isNaN(c) && (c = 0);
    var d = parseInt(c + (b - c) * sinuseinhundert[e]);
    a.text(d);
    100 > e && (e++, window.setTimeout(function () {
        slidevalues(a, c, b, e)
    }, 50 - 40 * Math.sin(Math.PI / 100 * e)))
}

function silentguildtag(a, c, b) {
    "undefined" != typeof guildcache[a] ? add_tag(c, b, guildcache[a][1], guildcache[a][2]) : jQuery.get("https://api.guildwars2.com/v1/guild_details.json?guild_id=" + a, function (e) {
        e = eval(e);
        guildcache[a] = [e.guild_name, e.tag, e.emblem];
        add_tag(c, b, e.tag, e.emblem)
    })
}

function guildlog(a, c, b, e) {
    "undefined" != typeof guildcache[c] ? (a = a.replace(/\[GUILD\]/, html_entities(guildcache[c][0] + " [" + guildcache[c][1] + "]")), addlog(a), add_tag(b, e, guildcache[c][1], guildcache[c][2]), "F17A37D1-3871-44BA-A3F1-95F045FCF227" == c && (speaking = !0, speaklist.push("wyld"), do_speak())) : jQuery.get("https://api.guildwars2.com/v1/guild_details.json?guild_id=" + c, function (d) {
        d = eval(d);
        guildcache[c] = [d.guild_name, d.tag, d.emblem];
        a = a.replace(/\[GUILD\]/, html_entities(d.guild_name + " [" + d.tag + "]"));
        addlog(a);
        add_tag(b, e, d.tag, d.emblem);
        "F17A37D1-3871-44BA-A3F1-95F045FCF227" == c && (speaking = !0, speaklist.push("wyld"), do_speak())
    })
}

function add_tag(a, c, b, e) {
    var d = new(L.Class.extend({
        initialize: function (a) {
            this._latlng = a
        },
        onAdd: function (a) {
            this._map = a;
            this._el = L.DomUtil.create("div", "guildtag leaflet-zoom-hide");
            jQuery(this._el).html("[TAG]");
            a.getPanes().markerPane.appendChild(this._el);
            a.on("viewreset", this._reset, this);
            this._reset()
        },
        onRemove: function (a) {
            a.getPanes().markerPane.removeChild(this._el);
            a.off("viewreset", this._reset, this)
        },
        _reset: function () {
            var a = this._map.latLngToLayerPoint(this._latlng);
            L.DomUtil.setPosition(this._el,
                a)
        }
    }))(a);
    if ("undefined" != typeof taglist)
        for (var g = 0; g < taglist.length; g++) "undefined" != typeof taglist[g] && taglist[g][0] == c && (map.removeLayer(taglist[g][1]), taglist[g] = void 0);
    taglist.push([c, d]);
    map.addLayer(d);
    jQuery(d._el).html("<span>[" + html_entities(b) + "]</span>").css({
        "z-index": 9E3
    });
    a = new(L.Class.extend({
        initialize: function (a) {
            this._latlng = a
        },
        onAdd: function (a) {
            this._map = a;
            this._el = L.DomUtil.create("div", "emblem leaflet-zoom-hide");
            a.getPanes().markerPane.appendChild(this._el);
            a.on("viewreset",
                this._reset, this);
            this._reset()
        },
        onRemove: function (a) {
            a.getPanes().markerPane.removeChild(this._el);
            a.off("viewreset", this._reset, this)
        },
        _reset: function () {
            var a = this._map.latLngToLayerPoint(this._latlng);
            L.DomUtil.setPosition(this._el, a)
        }
    }))(a);
    taglist.push([c, a]);
    map.addLayer(a);
    gw2emblem.init(a._el, 32, "transparent");
    gw2emblem.drawEmblemGw2(e);
    jQuery(a._el).css({
        "z-index": 9005
    })
}

function add_countdown(a, c) {
    var b = new(L.Class.extend({
        initialize: function (a) {
            this._latlng = a
        },
        onAdd: function (a) {
            this._map = a;
            this._el = L.DomUtil.create("div", "countdown leaflet-zoom-hide");
            jQuery(this._el).html("TIMER");
            a.getPanes().overlayPane.appendChild(this._el);
            a.on("viewreset", this._reset, this);
            this._reset()
        },
        onRemove: function (a) {
            a.getPanes().overlayPane.removeChild(this._el);
            a.off("viewreset", this._reset, this)
        },
        _reset: function () {
            var a = this._map.latLngToLayerPoint(this._latlng);
            L.DomUtil.setPosition(this._el,
                a)
        }
    }))(a),
        e = new Date;
    if ("undefined" != typeof countdownlist)
        for (var d = 0; d < countdownlist.length; d++) "undefined" != typeof countdownlist[d] && countdownlist[d][0] == c && (map.removeLayer(countdownlist[d][1]), countdownlist[d] = void 0);
    countdownlist.push([c, b, e.getTime() / 1E3]);
    "undefined" == typeof countdown_active && (countdown_active = window.setInterval(update_countdown, 100));
    map.addLayer(b)
}

function remove_countdown(a) {
    if ("undefined" != typeof countdownlist)
        for (var c = 0; c < countdownlist.length; c++) "undefined" != typeof countdownlist[c] && countdownlist[c][0] == a && (map.removeLayer(countdownlist[c][1]), countdownlist[c] = void 0)
}

function update_countdown() {
    for (var a = new Date, c = 0; c < countdownlist.length; c++)
        if ("undefined" != typeof countdownlist[c]) {
            var b = 0,
                b = 61 >= countdownlist[c][0] ? 300 : 120,
                e = b - Math.floor(a.getTime() / 1E3 - countdownlist[c][2]);
            0 < e ? (b = Math.floor(e / 60), e -= 60 * b, jQuery(countdownlist[c][1]._el).html("<span>" + b + ":" + (10 > e ? "0" : "") + e + "</span>")) : (map.removeLayer(countdownlist[c][1]), countdownlist[c] = void 0)
        }
    0 == countdownlist.length && (window.clearInterval(countdown_active), countdown_active = void 0)
}

function add_nodes() {
    var a = jQuery("#world").val();
    layer_nodes.clearLayers();
    0 == a ? (notify_user("Serverliste liegt noch nicht vor, bitte versuche es gleich noch einmal..."), jQuery("#layer-nodes").attr("checked", !1)) : jQuery.post("ajax.php", {
        server: a,
        action: "getnodes"
    }, function (a) {
        a = eval("(" + a + ")");
        addlog('Die Sammelpunkte der Crafting-Materialien entstammen der Seite <span style="color: #f0f;">http://gw2nodes.com</span> und werden dort gesammelt. Bitte schau mal dort vorbei und unterst\u00fctze das Projekt.',
            6E4, !1);
        if ("undefined" == typeof a.result && "undefined" == typeof a.result2) notify_user("keine Daten..."), jQuery("#layer-nodes").attr("checked", !1);
        else {
            for (var b = 0; b < a.result.length; b++) {
                var e;
                1 == a.result[b].map ? e = [
                    [10368, 23168],
                    [14464, 25216]
                ] : 2 == a.result[b].map ? e = [
                    [10112, 25216],
                    [12160, 29312]
                ] : 3 == a.result[b].map ? e = [
                    [20736, 8192],
                    [23808, 11264]
                ] : 4 == a.result[b].map && (e = [
                    [11520, 18944],
                    [14208, 20736]
                ]);
                var d = [e[0][0] + (e[1][0] - e[0][0]) * a.result[b].lati, e[0][1] + (e[1][1] - e[0][1]) * a.result[b].longi],
                    d = map.unproject(d,
                        map.getMaxZoom()),
                    g = icon_marker;
                1 == a.result[b].type && (g = icon_orichalcum);
                2 == a.result[b].type && (g = icon_ancient);
                3 == a.result[b].type && (g = icon_omnom);
                4 == a.result[b].type && (g = icon_snow);
                5 == a.result[b].type && (g = icon_orr);
                6 == a.result[b].type && (g = icon_passion);
                7 == a.result[b].type && (g = icon_himbeere);
                7 >= a.result[b].type && L.marker(d, {
                    icon: g
                }).addTo(layer_nodes)
            }
            for (b = 0; b < a.result2.length; b++) d = [a.result2[b].lati, a.result2[b].longi], add_usernode_to_map(d, a.result2[b].type, a.result2[b].id, a.result2[b].user, a.result2[b].username,
                a.result2[b].datum)
        }
    })
}

function number_to_month(a) {
    if (0 == a) return "Januar";
    if (1 == a) return "Februar";
    if (2 == a) return "M\u00e4rz";
    if (3 == a) return "April";
    if (4 == a) return "Mai";
    if (5 == a) return "Juni";
    if (6 == a) return "Juli";
    if (7 == a) return "August";
    if (8 == a) return "September";
    if (9 == a) return "Oktober";
    if (10 == a) return "November";
    if (11 == a) return "Dezember"
}

function timediff(a) {
    var c = (new Date).getTime();
    a = a.getTime();
    c = parseInt((c - a) / 1E3);
    if (60 > c) return c + " Sekunden";
    if (3600 > c) return parseInt(c / 60) + " Minuten";
    if (86400 > c) return parseInt(c / 3600) + " Stunden";
    if (86400 <= c) return parseInt(c / 86400) + " Tagen"
}

function add_usernode_to_map(a, c, b, e, d, g) {
    var h = icon_marker,
        p = "";
    "undefined" == typeof d && (d = "mir");
    "undefined" == typeof g && (g = (new Date).getTime() / 1E3);
    1 == c && (h = icon_geschmeidig, p = "geschmeidiger Holzblock");
    2 == c && (h = icon_abgelagert, p = "abgelagerter Holzblock");
    3 == c && (h = icon_hart, p = "harter Holzblock");
    p += "\nhinzugef\u00fcgt von \u00bb" + d + "\u00ab\nvor " + timediff(new Date(1E3 * g));
    if (3 >= c) {
        a = L.marker(a, {
            icon: h,
            title: p
        });
        if (rights.addnodes && userid == e || 1 == rights.deletenodes) jQuery(a).on("click", {
            marker: a,
            id: b
        }, delete_node);
        a.addTo(layer_nodes)
    }
}

function delete_node(a) {
    last_delete_node_clicked = a.data.marker;
    var c = '<a href="javascript:remove_node(' + a.data.id + ');"><img src ="images/adelete.png" /></a>';
    L.popup({
        maxWidth: 700
    }).setLatLng(a.data.marker.getLatLng()).setContent(c).openOn(map)
}

function remove_node(a, c) {
    map.closePopup();
    jQuery.post("ajax.php", {
        action: "removenode",
        id: a
    }, function (a) {
        1 == eval("(" + a + ")").result && map.removeLayer(last_delete_node_clicked);
        last_delete_node_clicked = void 0
    })
}

function addlog(a, c, b) {
    "undefined" == typeof b && (b = jQuery("#timestamp").prop("checked"));
    if (b) {
        var e = new Date;
        b = e.getHours();
        e = e.getMinutes();
        a = '<span class="timestamp">' + (10 > b ? "0" : "") + b + ":" + (10 > e ? "0" : "") + e + "</span> " + a
    }
    a = jQuery("<div />", {
        "class": "logentry",
        html: a
    });
    "undefined" == typeof c && (c = 6E4 * jQuery("#messagetime").slider("value") - 7500);
    a.appendTo("#wvwlog").delay(c).animate({
        "background-position": "0px +" + a.height() + "px"
    }, 1E3).fadeOut(1E4, function () {
        jQuery(this).remove()
    });
    window.setTimeout(function () {
        var a =
            jQuery("#wvwlog")[0].scrollHeight - jQuery("#wvwlog").height();
        jQuery("#wvwlog").animate({
            scrollTop: a
        })
    }, 300)
}

function changelog() {
    var a = parseInt(get_cookie("version"));
    0 < a && version > a && Mediabox.open("changelog.php?was=" + a, "Changelog");
    a != version && set_cookie("version", version)
}

function noapi() {
    "undefined" == typeof noapi_shown && (noapi_shown = !1);
    if (!noapi_shown) {
        disconnect_time = new Date;
        var a = "Keine Verbindung zur API seit " + (10 > disconnect_time.getHours() ? "0" : "") + disconnect_time.getHours() + ":" + (10 > disconnect_time.getMinutes() ? "0" : "") + disconnect_time.getMinutes() + ".";
        noapi_shown = !0;
        addlog("Keine Verbindung zur API...", 1E4, !1);
        jQuery("#map .content").text(a);
        jQuery("#noapi").fadeIn(5E3).mouseenter(function () {
            jQuery(this).stop(!0).animate({
                "background-color": "#fff",
                "border-color": "#000",
                width: "300px"
            }, 1500);
            jQuery(this).children(".content").stop(!0).delay(1500).fadeIn(2E3)
        }).mouseleave(function () {
            window.setTimeout(function () {
                jQuery("#noapi .content").fadeOut(300);
                jQuery("#noapi").delay(300).animate({
                    "background-color": "transparent",
                    width: "64px",
                    "border-color": "transparent"
                })
            }, 1E4)
        })
    }
}

function remove_noapi() {
    "undefined" == typeof noapi_shown && (noapi_shown = !1);
    noapi_shown && (noapi_shown = !1, jQuery("#noapi").off("mouseenter").off("mouseleave").stop(!0, !0).fadeOut(2E3))
}

function check_account(a) {
    return /^[a-zA-z\u00c0\u00c1\u00c2\u00c4\u00c6\u00c7\u00c8\u00c9\u00ca\u00cb\u00cd\u00ce\u00d1\u00d3\u00d4\u00d6\u0152\u00d9\u00da\u00db\u00dc\u00e0\u00e1\u00e2\u00e4\u00e6\u00e7\u00e8\u00e9\u00ea\u00eb\u00ed\u00ee\u00f1\u00f3\u00f4\u00f6\u0153\u00f9\u00fa\u00fb\u00fc\u00df ]+\.\d\d\d\d$/.test(a) ? !0 : !1
}

function register() {
    var a;
    a = '<p>F\u00fclle bitte alle Felder aus, wir werden uns dann mit Dir in Verbindung setzen, bevor wir Dich freischalten.</p><div id="errormessage" style="display: none;background-color:#fcc"></div><fieldset><label for="ptag">Accountname im Spiel (inkl. der vier Ziffern):</label><input type="text" class="text" id="ptag" /><br /><label for="flehen">kurzer Hinweis an uns, warum Du registriert werden m\u00f6chtest:</label><input type="text" class="text" id="flehen" /></fieldset>';
    jQuery("#dialog").html(a).dialog({
        title: "Registrieren",
        resizable: !1,
        modal: !0,
        width: 500,
        buttons: [{
            text: "lieber nicht",
            click: function () {
                jQuery(this).dialog("close");
                menuopen && togglemenu()
            }
        }, {
            text: "um Registrierung bitten",
            click: function () {
                jQuery("#errormessage").hide();
                var a = "",
                    b = !0,
                    e = jQuery("#ptag").val(),
                    d = jQuery("#flehen").val();
                check_account(e) || (b = !1, a += "<p>Kein g\u00fcltiger Account-Name!</p>");
                10 > d.length && (b = !1, a += "<p>Ohne Angabe eines Grundes wird keine Registrierung gew\u00e4hrt!</p>");
                b ? (jQuery(this).dialog("close"), menuopen && togglemenu(), jQuery.post("ajax.php", {
                    action: "ask_register",
                    tag: e,
                    reason: d
                }, function (a) {
                    a = eval("(" + a + ")");
                    1 == a.result ? notify_user("Wir haben Dein Ersuchen erhalten und werden uns demn\u00e4chst bei Dir melden.") : notify_user(html_entities(a.result))
                })) : jQuery("#errormessage").html(a).show()
            }
        }],
        show: {
            effect: "drop",
            duraction: 1E3
        },
        hide: {
            effect: "explode",
            duration: 1E3
        }
    })
}

function html_entities(a) {
    return String(a).replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;")
}

function notify_user(a) {
    jQuery("#messagebox").append("<p>" + a + "</p>").fadeIn();
    window.clearTimeout(mb_timer);
    mb_timer = window.setTimeout(hide_messagebox, 1E4)
}

function hide_messagebox() {
    window.clearTimeout(mb_timer);
    mb_timer = window.setTimeout(clear_messagebox, 2100);
    jQuery("#messagebox").fadeOut(2E3)
}

function clear_messagebox() {
    window.clearTimeout(mb_timer);
    jQuery("#messagebox").html("")
}

function get_cookie(a) {
    if (!document.cookie) return "";
    for (var c = document.cookie.split(";"), b = 0; b < c.length; b++) {
        var e = c[b].split("=");
        if (e[0].trim() == a) return e[1]
    }
    return ""
}

function set_cookie(a, c) {
    var b = new Date;
    b.setTime(b.getTime() + 31536E6);
    document.cookie = a + "=" + c + "; expires=" + b.toGMTString()
};