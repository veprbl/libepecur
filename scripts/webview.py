#!/usr/bin/env python

import os

import web
from web import form

urls = (
  '/', 'index'
)

PLOTS_PATH = "plots/"

items = set()
for path, dirs, files in os.walk(PLOTS_PATH):
    path = path[len(PLOTS_PATH):]
    path = path.split("/")
    if len(path) < 2:
        continue
    path = os.path.join(*path[1:])
    files = filter(lambda f: f.endswith(".png"), files)
    files = map(lambda f: os.path.join(path, f), files)
    items.update(files)

args = map(lambda f: (f,f), items)
args.sort()
items = [
    form.Dropdown(name='plots', multiple='multiple', size=len(args), args=args, value=[])
]
items.append(form.Button("submit", type="submit"))
plot_selector = form.Form(*items)

class index:
    def GET(self):
        with open("list_apr10", "r") as fp:
                lines = fp.readlines()
        runs = map(lambda l: l.split(), lines)
        runs.sort(key=lambda tup: int(tup[1]))
        run_names = map(lambda tup: tup[0], runs)
        form = plot_selector()
        form.validates()
        res = ""
        res += """<pre>%s</pre>""" % web.htmlquote(repr(form["plots"].value))
        res += """<form method="POST">%s</form>""" % form.render()
        res += "<table>"
        for run_name in run_names:
            res += "<tr><th>%s</th>" % run_name
            if not type(form["plots"].value) is list:
                # workaround for not patched webpy
                form["plots"].value = [form["plots"].value]
            for plotname in form["plots"].value:
                res += "<td>"
                res += """<img src="/plots/%s/%s" />""" % (run_name, plotname)
                res += "</td>"
            res += "</tr>"
        res += "</table>"
        web.header('Content-Type', 'text/html')
        return res

    def POST(self):
        return self.GET()

if __name__ == "__main__":
    web.application(urls, globals()).run()
