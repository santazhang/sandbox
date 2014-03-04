#!/usr/bin/env coffee

Number::formatMoney = (t=',', d='.', c='') ->
    n = this
    s = if n < 0 then "-#{c}" else c
    i = Math.abs(n).toFixed(0)
    j = (if (j = i.length) > 3 then j % 3 else 0)
    s += i.substr(0, j) + t if j
    return s + i.substr(j).replace(/(\d{3})(?=\d)/g, "$1" + t)

now = ->
    (new Date).getTime() / 1000.0

ctr = 0

start = now()

while (true)
    if (now() - start > 1.0)
        break
    i = 0
    while i < 100000
        i++
        ctr += 1

console.log "int++", parseInt(ctr / (now() - start)).formatMoney()
