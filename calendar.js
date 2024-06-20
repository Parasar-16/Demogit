(self.webpackChunk_N_E = self.webpackChunk_N_E || []).push([[4844], {
    63317: function(e, n, t) {
        (window.__NEXT_P = window.__NEXT_P || []).push(["/calendar", function() {
            return t(63823)
        }
        ])
    },
    28407: function(e, n, t) {
        "use strict";
        var a = t(828)
          , r = t(85893)
          , i = t(67294)
          , l = t(9645)
          , s = t(38538)
          , c = t(56841)
          , o = t(74684)
          , d = t(67273)
          , u = t(26095)
          , h = t(28702);
        n.Z = function(e) {
            var n = e.classroom
              , t = e.handleClassroom
              , f = e.classroomList
              , m = e.schedule
              , v = e.handleSchedule
              , x = e.scheduleList
              , g = e.profile
              , j = e.handleProfile
              , p = e.isAdminCalendar
              , y = (0,
            a.Z)(i.useState(!1), 2)
              , N = y[0]
              , b = y[1]
              , w = (0,
            a.Z)(i.useState(null), 2)
              , Z = (w[0],
            w[1])
              , k = (0,
            a.Z)(i.useState("schedule"), 2)
              , D = k[0]
              , S = k[1];
            i.useEffect((function() {
                f && f.length > 0 && S("classroom")
            }
            ), [f]);
            var C = function(e) {
                var n;
                if (x && x.length > 0) {
                    var t = x.filter((function(n) {
                        return n.schedule.id === e
                    }
                    ));
                    t.length > 0 && (n = t[0].schedule)
                }
                return n
            };
            return (0,
            r.jsxs)("div", {
                className: "global-calendar-filter-wrapper",
                children: [!n && !m && !g && (0,
                r.jsx)("div", {
                    className: "global-calendar-filter-item",
                    children: (0,
                    r.jsx)("div", {
                        className: "global-calendar-filter-item-text",
                        children: "All"
                    })
                }), n && (0,
                r.jsxs)("div", {
                    className: "global-calendar-filter-item",
                    children: [(0,
                    r.jsx)("div", {
                        className: "global-calendar-filter-item-icon"
                    }), (0,
                    r.jsx)("div", {
                        className: "global-calendar-filter-item-text",
                        children: function(e) {
                            if (f && f.length > 0) {
                                var n = f.find((function(n, t) {
                                    return n.id === parseInt(e)
                                }
                                ));
                                if (n)
                                    return "".concat(n.name)
                            }
                        }(n)
                    }), (0,
                    r.jsx)("div", {
                        className: "global-calendar-filter-item-times",
                        onClick: function() {
                            return t("")
                        },
                        children: (0,
                        r.jsx)(o.u, {})
                    })]
                }), m && (0,
                r.jsxs)("div", {
                    className: "global-calendar-filter-item",
                    children: [(0,
                    r.jsxs)("div", {
                        className: "global-calendar-filter-item-icon d-flex",
                        children: [(0,
                        r.jsx)(u.f, {
                            style: {
                                color: "".concat(C(m).color),
                                background: "".concat(C(m).color)
                            }
                        }), " "]
                    }), (0,
                    r.jsx)("div", {
                        className: "global-calendar-filter-item-text",
                        children: C(m).name
                    }), (0,
                    r.jsx)("div", {
                        className: "global-calendar-filter-item-times",
                        onClick: function() {
                            return v("")
                        },
                        children: (0,
                        r.jsx)(o.u, {})
                    })]
                }), g && (0,
                r.jsxs)("div", {
                    className: "global-calendar-filter-item",
                    children: [(0,
                    r.jsx)("div", {
                        className: "global-calendar-filter-item-icon"
                    }), (0,
                    r.jsx)("div", {
                        className: "global-calendar-filter-item-text",
                        children: g.label
                    }), (0,
                    r.jsx)("div", {
                        className: "global-calendar-filter-item-times",
                        onClick: function() {
                            return j("")
                        },
                        children: (0,
                        r.jsx)(o.u, {})
                    })]
                }), (0,
                r.jsx)("div", {
                    className: "global-calendar-filter-item global-calendar-filter-dropdown",
                    children: (0,
                    r.jsxs)(l.Z, {
                        show: N,
                        onToggle: function() {
                            b(!N)
                        },
                        className: "p-0 m-0",
                        children: [(0,
                        r.jsx)(l.Z.Toggle, {
                            as: "div",
                            className: "p-0 m-0",
                            children: (0,
                            r.jsxs)("div", {
                                className: "global-calendar-dropdown-item",
                                children: [(0,
                                r.jsx)("div", {
                                    className: "icon",
                                    children: (0,
                                    r.jsx)(d.v, {})
                                }), (0,
                                r.jsx)("div", {
                                    className: "text",
                                    children: "Add Filter"
                                })]
                            })
                        }), (0,
                        r.jsx)(l.Z.Menu, {
                            className: "p-0",
                            children: (0,
                            r.jsx)("div", {
                                className: "global-calendar-content-wrapper",
                                children: (0,
                                r.jsxs)(s.Z, {
                                    id: "tabs",
                                    activeKey: D,
                                    onSelect: function(e) {
                                        S(e)
                                    },
                                    children: [f && f.length > 0 && (0,
                                    r.jsx)(c.Z, {
                                        eventKey: "classroom",
                                        title: "Classroom",
                                        children: (0,
                                        r.jsx)("div", {
                                            children: (0,
                                            r.jsx)(h.Js, {
                                                data: n,
                                                handleData: t,
                                                list: f
                                            })
                                        })
                                    }, "classroom"), x && x.length > 0 && (0,
                                    r.jsx)(c.Z, {
                                        eventKey: "schedule",
                                        title: "Schedules",
                                        children: (0,
                                        r.jsx)("div", {
                                            children: (0,
                                            r.jsx)(h.Go, {
                                                data: m,
                                                handleData: v,
                                                list: x
                                            })
                                        })
                                    }, "schedule"), p && (0,
                                    r.jsx)(c.Z, {
                                        eventKey: "users",
                                        title: "Users",
                                        children: (0,
                                        r.jsx)("div", {
                                            children: (0,
                                            r.jsx)(h.CT, {
                                                data: g,
                                                handleData: j,
                                                SetUserOptions: Z
                                            })
                                        })
                                    }, "users")]
                                })
                            })
                        })]
                    })
                })]
            })
        }
    },
    65483: function(e, n, t) {
        "use strict";
        t.d(n, {
            AT: function() {
                return c
            },
            Wb: function() {
                return u
            },
            bL: function() {
                return o
            },
            co: function() {
                return s
            },
            h$: function() {
                return d
            },
            ks: function() {
                return i
            },
            l_: function() {
                return l
            }
        });
        var a = t(80008)
          , r = t.n(a)
          , i = [{
            key: "Jan",
            fullName: "January"
        }, {
            key: "Feb",
            fullName: "February"
        }, {
            key: "Mar",
            fullName: "March"
        }, {
            key: "Apr",
            fullName: "April"
        }, {
            key: "May",
            fullName: "May"
        }, {
            key: "Jun",
            fullName: "June"
        }, {
            key: "Jul",
            fullName: "July"
        }, {
            key: "Aug",
            fullName: "August"
        }, {
            key: "Sep",
            fullName: "September"
        }, {
            key: "Oct",
            fullName: "October"
        }, {
            key: "Nov",
            fullName: "November"
        }, {
            key: "Dec",
            fullName: "December"
        }]
          , l = [{
            key: "Sun",
            fullName: "Sunday"
        }, {
            key: "Mon",
            fullName: "Monday"
        }, {
            key: "Tue",
            fullName: "Tuesday"
        }, {
            key: "Wed",
            fullName: "Wednesday"
        }, {
            key: "Thu",
            fullName: "Thursday"
        }, {
            key: "Fri",
            fullName: "Friday"
        }, {
            key: "Sat",
            fullName: "Saturday"
        }]
          , s = function(e) {
            return e > 9 ? e : "0".concat(e)
        }
          , c = function(e) {
            var n = new Date(e)
              , t = s(n.getDate())
              , a = s(n.getMonth() + 1)
              , r = s(n.getFullYear());
            return "".concat(t, "-").concat(a, "-").concat(r)
        }
          , o = function(e) {
            var n = new Date(e)
              , t = s(n.getDate())
              , a = i[n.getMonth()].fullName
              , r = l[n.getDay()].fullName;
            return "".concat(r, ", ").concat(a, " ").concat(t)
        }
          , d = function(e) {
            var n = new Date(e)
              , t = s(n.getHours())
              , a = t >= 12 ? "PM" : "AM";
            t = (t %= 12) || 12;
            var r = s(n.getMinutes());
            s(n.getSeconds());
            return "".concat(t, ":").concat(r, " ").concat(a)
        }
          , u = function(e) {
            var n, t = new Date(e), a = r().tz.guess();
            return null === (n = r().tz.zone(a)) || void 0 === n ? void 0 : n.abbr(t.getTimezoneOffset())
        }
    },
    89502: function(e, n, t) {
        "use strict";
        t.d(n, {
            Z: function() {
                return m
            }
        });
        var a = t(85893)
          , r = t(67294)
          , i = t(5152)
          , l = t.n(i)
          , s = t(7542)
          , c = t(5979)
          , o = t(62990)
          , d = t(14614)
          , u = t(66151)
          , h = function() {
            return (0,
            r.useEffect)((function() {
                (null === window || void 0 === window ? void 0 : window.HNWidget) || function() {
                    var e = document
                      , n = e.createElement("script");
                    n.src = "https://edisonos.hellonext.co/widget.js",
                    n.id = "changelog-script",
                    n.onload = function() {
                        var e;
                        null === window || void 0 === window || null === (e = window.HNWidget) || void 0 === e || e.init({
                            token: "D-jYQp7cny6Mimx-3NkoaA",
                            selector: "#changelogWidgetID",
                            widget: "changelog",
                            placement: "bottom",
                            enableIndicator: !0
                        })
                    }
                    ,
                    e.getElementsByTagName("head")[0].appendChild(n)
                }()
            }
            ), []),
            (0,
            a.jsx)(a.Fragment, {})
        }
          , f = l()((function() {
            return t.e(4336).then(t.bind(t, 14336))
        }
        ), {
            loadableGenerated: {
                webpack: function() {
                    return [14336]
                }
            },
            ssr: !1
        })
          , m = function(e) {
            var n, t, i = r.useContext(u.m), l = i.globalState;
            i.globalDispatch;
            return r.useEffect((function() {
                (0,
                d.NK)(o.Hr)
            }
            ), []),
            (0,
            a.jsxs)("div", {
                children: [(0,
                a.jsx)(f, {}), (0,
                a.jsx)(h, {}), (0,
                a.jsxs)("div", {
                    className: "admin-layout-wrapper",
                    children: [(0,
                    a.jsx)("div", {
                        className: "admin-layout-top-wrapper",
                        children: (0,
                        a.jsx)(c.Z, {})
                    }), (0,
                    a.jsxs)("div", {
                        className: "admin-layout-bottom-wrapper",
                        children: [!e.noSidebar && (0,
                        a.jsx)("div", {
                            className: "admin-layout-bottom-left-wrapper ".concat(l.sidebarToggle ? "toggle" : ""),
                            children: e.sidebar ? e.sidebar : (0,
                            a.jsx)(s.Z, {
                                tenantAccessToken: null === (n = e.children) || void 0 === n || null === (t = n.props) || void 0 === t ? void 0 : t.tenantAccessToken
                            })
                        }), (0,
                        a.jsx)("div", {
                            id: "scroll",
                            className: "admin-layout-bottom-right-wrapper",
                            children: e.children
                        })]
                    })]
                })]
            })
        }
    },
    70207: function(e, n, t) {
        "use strict";
        var a = t(85893)
          , r = (t(67294),
        t(10682))
          , i = t(80168)
          , l = t(44665);
        n.Z = function(e) {
            var n = e.noContainer
              , t = void 0 !== n && n
              , s = e.container
              , c = e.padding
              , o = e.children
              , d = "column";
            return (0,
            a.jsxs)(l.ve, {
                direction: d,
                children: [(0,
                a.jsx)(i.C9, {}), c >= 0 ? (0,
                a.jsx)(l.oY, {
                    padding: c,
                    children: o
                }) : (0,
                a.jsx)(a.Fragment, {
                    children: t ? (0,
                    a.jsx)(a.Fragment, {
                        children: o
                    }) : (0,
                    a.jsx)(l.oY, {
                        children: (0,
                        a.jsx)(r.Z, {
                            fluid: !!s,
                            children: o
                        })
                    })
                })]
            })
        }
    },
    27194: function(e, n, t) {
        "use strict";
        var a = t(85893)
          , r = (t(67294),
        t(10682))
          , i = t(41330)
          , l = t(30159)
          , s = t(56145)
          , c = t(86512)
          , o = t(11163);
        n.Z = function(e) {
            var n = e.children
              , t = (0,
            o.useRouter)();
            return (0,
            a.jsxs)(a.Fragment, {
                children: [(0,
                a.jsx)(s.OC, {
                    children: (0,
                    a.jsx)(r.Z, {
                        children: (0,
                        a.jsxs)(s.Ej, {
                            align: "center",
                            style: {
                                height: "70px"
                            },
                            children: [(0,
                            a.jsx)(l.Uf, {
                                href: "/",
                                children: (0,
                                a.jsx)(i.Z, {
                                    style: {
                                        height: "50px",
                                        objectFit: "contain"
                                    },
                                    alt: "logo",
                                    src: function() {
                                        if (null === e || void 0 === e ? void 0 : e.tenantDetails) {
                                            var n = e.tenantDetails;
                                            return n && n.info && Object.keys(n.info).length > 0 && n.info.logo.length > 0 ? n.info.logo : "https://edison-tenant.b-cdn.net/Empty-states/Random%20Logo.svg"
                                        }
                                        return "https://edison-tenant.b-cdn.net/Empty-states/Random%20Logo.svg"
                                    }(),
                                    width: "150"
                                })
                            }), (0,
                            a.jsx)(s.B5, {
                                style: {
                                    marginLeft: "auto"
                                },
                                children: (0,
                                a.jsx)("div", {
                                    className: "hide-in-mobile",
                                    style: {
                                        fontSize: "14px"
                                    },
                                    children: t.pathname.includes("/signin") ? (0,
                                    a.jsxs)("div", {
                                        style: {
                                            display: "flex"
                                        },
                                        children: ["Don't have an account? \xa0", (0,
                                        a.jsx)(l.Uf, {
                                            href: "/signup",
                                            children: (0,
                                            a.jsx)("div", {
                                                children: (0,
                                                a.jsx)(c.e9, {
                                                    children: "Create account"
                                                })
                                            })
                                        })]
                                    }) : (0,
                                    a.jsxs)("div", {
                                        style: {
                                            display: "flex"
                                        },
                                        children: ["Already have an account? \xa0", (0,
                                        a.jsx)(l.Uf, {
                                            href: "/signin",
                                            children: (0,
                                            a.jsx)("div", {
                                                children: (0,
                                                a.jsx)(c.e9, {
                                                    children: "Login"
                                                })
                                            })
                                        })]
                                    })
                                })
                            })]
                        })
                    })
                }), n]
            })
        }
    },
    56998: function(e, n, t) {
        "use strict";
        t.d(n, {
            lq: function() {
                return l.Z
            },
            zv: function() {
                return b
            },
            hG: function() {
                return i.Z
            },
            n6: function() {
                return s
            }
        });
        var a = t(85893)
          , r = t(67294)
          , i = t(70207)
          , l = t(89502)
          , s = function(e) {
            var n = e.children;
            return (0,
            a.jsx)(a.Fragment, {
                children: n
            })
        }
          , c = (t(27194),
        t(47568))
          , o = t(828)
          , d = t(70655)
          , u = t(10682)
          , h = t(41330)
          , f = t(94955)
          , m = t(35005)
          , v = t(30159)
          , x = t(99287)
          , g = t(54629)
          , j = t(33518)
          , p = t(56145)
          , y = t(54737)
          , N = t(81942)
          , b = function(e) {
            var n = e.children
              , t = (0,
            o.Z)(r.useState(!1), 2)
              , i = t[0]
              , l = t[1]
              , s = function() {
                return l(!1)
            };
            function b() {
                return (b = (0,
                c.Z)((function() {
                    var e, n;
                    return (0,
                    d.__generator)(this, (function(t) {
                        switch (t.label) {
                        case 0:
                            return e = "",
                            (0,
                            x.LP)() && (n = JSON.parse((0,
                            x.LP)()),
                            e = n.refresh_token),
                            [4, (0,
                            j.le)(g.Q3$, {
                                refresh_token: e
                            })];
                        case 1:
                            return t.sent(),
                            (0,
                            x.kS)(),
                            [2]
                        }
                    }
                    ))
                }
                ))).apply(this, arguments)
            }
            return (0,
            a.jsxs)(a.Fragment, {
                children: [(0,
                a.jsx)(p.OC, {
                    children: (0,
                    a.jsx)(u.Z, {
                        children: (0,
                        a.jsxs)(p.Ej, {
                            align: "center",
                            style: {
                                height: "70px"
                            },
                            children: [(0,
                            a.jsx)(p.B5, {
                                children: (0,
                                a.jsx)(v.Uf, {
                                    href: "/",
                                    children: (0,
                                    a.jsx)(h.Z, {
                                        style: {
                                            height: "50px",
                                            objectFit: "contain"
                                        },
                                        alt: "logo",
                                        src: function() {
                                            if (null === e || void 0 === e ? void 0 : e.tenantDetails) {
                                                var n = e.tenantDetails;
                                                return n && n.info && Object.keys(n.info).length > 0 && n.info.logo.length > 0 ? n.info.logo : "https://edison-tenant.b-cdn.net/Empty-states/Random%20Logo.svg"
                                            }
                                            return "https://edison-tenant.b-cdn.net/Empty-states/Random%20Logo.svg"
                                        }(),
                                        width: "150"
                                    })
                                })
                            }), (0,
                            a.jsxs)(p.B5, {
                                style: {
                                    marginLeft: "auto",
                                    cursor: "pointer",
                                    fontWeight: 600,
                                    color: "red",
                                    fontSize: "14px"
                                },
                                onClick: function() {
                                    return l(!0)
                                },
                                children: [(0,
                                a.jsx)(N.R, {
                                    size: "16px",
                                    color: "red"
                                }), "\xa0 Sign Out"]
                            })]
                        })
                    })
                }), n, (0,
                a.jsx)(f.Z, {
                    dialogClassName: "modal-custom-width",
                    centered: !0,
                    show: i,
                    onHide: s,
                    backdrop: "static",
                    keyboard: !1,
                    children: (0,
                    a.jsxs)(f.Z.Body, {
                        className: "p-4",
                        children: [(0,
                        a.jsxs)(p.wC, {
                            children: [(0,
                            a.jsx)("div", {
                                className: "secondary-heading pb-2",
                                children: "Are you sure you want to Log Out?"
                            }), (0,
                            a.jsx)(p.Pc, {
                                onClick: s,
                                children: (0,
                                a.jsx)(y.x, {})
                            })]
                        }), (0,
                        a.jsx)(p.f3, {
                            children: (0,
                            a.jsxs)("div", {
                                className: "text-right mt-3",
                                children: [(0,
                                a.jsx)(m.Z, {
                                    className: "primary-medium-button secondary mr-2",
                                    onClick: s,
                                    children: "Go Back"
                                }), (0,
                                a.jsx)(m.Z, {
                                    className: "primary-medium-button danger",
                                    onClick: function() {
                                        !function() {
                                            b.apply(this, arguments)
                                        }()
                                    },
                                    children: "Logout"
                                })]
                            })
                        })]
                    })
                })]
            })
        }
    },
    37525: function(e, n, t) {
        "use strict";
        var a = t(47568)
          , r = t(26042)
          , i = t(69396)
          , l = t(70655)
          , s = t(85893)
          , c = t(99287)
          , o = t(64991)
          , d = t(11163)
          , u = t(67294)
          , h = function(e) {
            return !0 === e.forbidden ? (0,
            s.jsx)(s.Fragment, {}) : (0,
            s.jsx)("div", {
                children: e.children
            })
        };
        n.Z = function(e) {
            var n = function(n) {
                var t = (0,
                d.useRouter)();
                return u.useEffect((function() {
                    if (n.forbidden) {
                        var e = window.location.href;
                        return (0,
                        c.UU)(e),
                        void t.push({
                            pathname: "/signin"
                        })
                    }
                }
                ), [n.forbidden, n.tokenDetails]),
                e.Layout ? (0,
                s.jsx)(h, (0,
                i.Z)((0,
                r.Z)({}, n), {
                    children: (0,
                    s.jsx)(e.Layout, {
                        children: (0,
                        s.jsx)(e, (0,
                        r.Z)({}, n))
                    })
                })) : (0,
                s.jsx)(h, (0,
                i.Z)((0,
                r.Z)({}, n), {
                    children: (0,
                    s.jsx)(e, (0,
                    r.Z)({}, n))
                }))
            };
            return n.getInitialProps = function() {
                var n = (0,
                a.Z)((function(n) {
                    var t, a, s, d, u, h;
                    return (0,
                    l.__generator)(this, (function(l) {
                        switch (l.label) {
                        case 0:
                            return [4, (0,
                            c.bW)(n)];
                        case 1:
                            return t = l.sent(),
                            [4, (0,
                            c.qd)(n)];
                        case 2:
                            return a = l.sent(),
                            n && n.req ? n.req.headers && n.req.headers.host && (d = n.req.headers.host) : d = window.location.host,
                            d && (s = (f = d).includes("localhost") ? "guru" : f.split(".")[0]),
                            (h = e.getInitialProps) ? [4, e.getInitialProps(n, {
                                tenant_name: s
                            })] : [3, 4];
                        case 3:
                            h = l.sent(),
                            l.label = 4;
                        case 4:
                            return u = h,
                            t && t.user ? t.user.is_active ? a ? [2, (0,
                            i.Z)((0,
                            r.Z)({}, u), {
                                tokenDetails: t,
                                userProfileDetails: a
                            })] : ((0,
                            o.Z)(n, "/profile-init"),
                            [2, {}]) : ((0,
                            o.Z)(n, "/signin"),
                            [2, {}]) : (null === u || void 0 === u ? void 0 : u.exemptAuthentication) ? [2, (0,
                            r.Z)({}, u)] : [2, {
                                forbidden: !0
                            }]
                        }
                        var f
                    }
                    ))
                }
                ));
                return function(e) {
                    return n.apply(this, arguments)
                }
            }(),
            n
        }
    },
    63823: function(e, n, t) {
        "use strict";
        t.r(n);
        var a = t(828)
          , r = t(85893)
          , i = t(67294)
          , l = t(9008)
          , s = t.n(l)
          , c = t(10682)
          , o = t(54368)
          , d = t(44134)
          , u = t(56998)
          , h = t(28702)
          , f = t(28407)
          , m = t(50834)
          , v = t(33518)
          , x = t(54629)
          , g = t(65483)
          , j = t(37525);
        n.default = (0,
        j.Z)((function() {
            var e = i.useRef(null)
              , n = (0,
            a.Z)(i.useState(), 2)
              , t = n[0]
              , l = n[1]
              , j = (0,
            a.Z)(i.useState(""), 2)
              , p = j[0]
              , y = j[1]
              , N = (0,
            a.Z)(i.useState(""), 2)
              , b = N[0]
              , w = N[1]
              , Z = (0,
            a.Z)(i.useState(null), 2)
              , k = Z[0]
              , D = Z[1]
              , S = (0,
            a.Z)(i.useState(!1), 2)
              , C = S[0]
              , _ = S[1]
              , T = (0,
            a.Z)(i.useState("day"), 2)
              , F = T[0]
              , A = T[1]
              , E = (0,
            a.Z)(i.useState(String), 2)
              , L = E[0]
              , M = E[1]
              , O = (0,
            a.Z)(i.useState(String), 2)
              , P = O[0]
              , R = O[1]
              , J = (0,
            a.Z)(i.useState(String), 2)
              , z = J[0]
              , G = J[1]
              , H = (0,
            a.Z)(i.useState(""), 2)
              , U = H[0]
              , W = H[1]
              , I = i.useCallback((function(e, n, t, a, r) {
                if ("day" === n) {
                    var i = new Date(e);
                    R(""),
                    G(""),
                    M(i),
                    q(i, P, z, t, a, r, n)
                }
                if ("week" === n) {
                    var l = new Date(e)
                      , s = new Date(e)
                      , c = new Date(e);
                    s.setDate(l.getDate() - l.getDay()),
                    c.setDate(l.getDate() + 6);
                    var o = new Date(s)
                      , d = new Date(c);
                    M(l),
                    R(o),
                    G(d),
                    q(l, s, c, t, a, r, n)
                }
                if ("month" === n) {
                    var u = new Date(e)
                      , h = new Date(u.getFullYear(),u.getMonth(),1)
                      , f = new Date(u.getFullYear(),u.getMonth() + 1,0);
                    M(u),
                    R(h),
                    G(f),
                    q(u, h, f, t, a, r, n)
                }
            }
            ), [z, P])
              , Y = i.useCallback((function(e) {
                var n = (0,
                m.CR)();
                (null === n || void 0 === n ? void 0 : n.id) && (D(null === n || void 0 === n ? void 0 : n.id),
                I(e, F, p, null === n || void 0 === n ? void 0 : n.id, b))
            }
            ), [p, F, I, b])
              , q = function(e, n, t, a, r, i, l) {
                var s = e ? "date=".concat((0,
                g.AT)(e)) : ""
                  , c = n ? "start_date=".concat((0,
                g.AT)(n)) : ""
                  , o = t ? "end_date=".concat((0,
                g.AT)(t)) : ""
                  , d = a ? "room_id=".concat(a) : ""
                  , u = r ? "profile_id=".concat(r) : ""
                  , h = i ? "schedule=".concat(i) : ""
                  , f = "";
                f = "day" === l ? "".concat(s || "").concat(d ? "&" + d : "").concat(u ? "&" + u : "").concat(h ? "&" + h : "") : "".concat(c || "").concat(o ? "&" + o : "").concat(d ? "&" + d : "").concat(u ? "&" + u : "").concat(h ? "&" + h : ""),
                W(f)
            }
              , B = (0,
            d.ZP)(k ? [x.TH1(k), k] : null, v.M4, {
                refreshInterval: 0,
                revalidateOnFocus: !1
            })
              , K = B.data
              , Q = (B.error,
            (0,
            d.ZP)(U ? [x.GT_(U), U] : null, (function(e) {
                return (0,
                v.M4)(e)
            }
            )))
              , X = Q.data;
            Q.error;
            return i.useEffect((function() {
                if (K && K.length > 0) {
                    var e = [];
                    K.map((function(n) {
                        e.push(n.room)
                    }
                    )),
                    l(e)
                }
            }
            ), [K]),
            (0,
            r.jsxs)(u.hG, {
                container: !0,
                padding: 0,
                children: [(0,
                r.jsx)(s(), {
                    children: (0,
                    r.jsx)("title", {
                        children: "Calendar"
                    })
                }), (0,
                r.jsx)(c.Z, {
                    style: {
                        padding: "20px",
                        minHeight: "500px"
                    },
                    children: (0,
                    r.jsxs)("div", {
                        className: "global-calendar-root-wrapper",
                        children: [(0,
                        r.jsxs)("div", {
                            className: "calender-header",
                            children: [(0,
                            r.jsx)("div", {
                                className: "header-item",
                                children: (0,
                                r.jsxs)("div", {
                                    className: "calendar-dropdown-wrapper",
                                    children: [(0,
                                    r.jsx)("div", {
                                        className: "dropdown-icon",
                                        onClick: function() {
                                            return _(!C)
                                        },
                                        children: (0,
                                        r.jsx)(o.f, {})
                                    }), (0,
                                    r.jsx)("div", {
                                        className: "dropdown-content " + (C ? "active" : ""),
                                        children: (0,
                                        r.jsx)(h.wr, {
                                            currentDate: L,
                                            clickOnDate: function() {
                                                return _(!C)
                                            },
                                            renderView: F,
                                            handleCurrentDate: Y,
                                            ref: e
                                        })
                                    })]
                                })
                            }), (0,
                            r.jsxs)("div", {
                                className: "header-item",
                                children: [L && (0,
                                r.jsx)("div", {
                                    className: "calendar-description",
                                    children: function() {
                                        var e, n = new Date(L);
                                        return "".concat(null === (e = g.l_[n.getDay()]) || void 0 === e ? void 0 : e.fullName)
                                    }()
                                }), L && (0,
                                r.jsx)("div", {
                                    className: "calendar-heading",
                                    children: function() {
                                        var e = new Date(L);
                                        return "".concat(e.getDate(), " ").concat(g.ks[e.getMonth()].fullName, " ").concat(e.getFullYear())
                                    }()
                                })]
                            }), (0,
                            r.jsx)("div", {
                                className: "header-item render-view ml-auto",
                                children: ["day", "week", "month"].map((function(e, n) {
                                    return (0,
                                    r.jsx)("div", {
                                        className: "render-view-item ".concat(F === e ? "active" : ""),
                                        onClick: function() {
                                            return A(n = e),
                                            void I(L, n, p, k, b);
                                            var n
                                        },
                                        children: e
                                    }, e)
                                }
                                ))
                            })]
                        }), (0,
                        r.jsx)("hr", {
                            className: "m-0 p-0 my-3"
                        }), (0,
                        r.jsx)("div", {
                            children: (0,
                            r.jsx)(f.Z, {
                                classroom: p,
                                handleClassroom: function(e) {
                                    if (y(e),
                                    I(L, F, e, k, b),
                                    e)
                                        w(""),
                                        (0,
                                        d.JG)([x.GT_(U), U]);
                                    else {
                                        var n = (0,
                                        m.CR)();
                                        (null === n || void 0 === n ? void 0 : n.id) && (D(null === n || void 0 === n ? void 0 : n.id),
                                        I(L, F, e, null === n || void 0 === n ? void 0 : n.id, ""))
                                    }
                                },
                                classroomList: t
                            })
                        }), (0,
                        r.jsx)("hr", {
                            className: "m-0 p-0 my-3"
                        }), (0,
                        r.jsx)("div", {
                            style: {
                                marginTop: "10px"
                            },
                            children: "day" === F ? (0,
                            r.jsx)(h.yR, {
                                sessions: X,
                                dateQuery: U,
                                room: p,
                                profile: k,
                                role: "student",
                                render: F,
                                view: "user-global-calendar"
                            }) : "week" === F ? (0,
                            r.jsx)(h.L5, {
                                sessions: X,
                                currentDate: L,
                                startDate: P,
                                endDate: z,
                                dateQuery: U,
                                room: p,
                                profile: k,
                                role: "student",
                                render: F,
                                view: "user-global-calendar"
                            }) : (0,
                            r.jsx)(h.H1, {
                                sessions: X,
                                currentDate: L,
                                startDate: P,
                                endDate: z,
                                dateQuery: U,
                                room: p,
                                profile: k,
                                role: "student",
                                render: F,
                                view: "user-global-calendar"
                            })
                        })]
                    })
                })]
            })
        }
        ))
    }
}, function(e) {
    e.O(0, [8523, 806, 1265, 5865, 9645, 4134, 7536, 8764, 1328, 350, 1033, 4981, 2889, 3572, 4231, 7502, 7284, 3023, 5857, 5994, 7941, 9958, 5513, 4497, 9705, 1763, 5600, 8953, 8702, 9774, 2888, 179], (function() {
        return n = 63317,
        e(e.s = n);
        var n
    }
    ));
    var n = e.O();
    _N_E = n
}
]);