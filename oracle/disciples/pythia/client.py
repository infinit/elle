# -*- encoding: utf-8 -*-

import os
import json
import httplib2
try:
    import urllib2
except:
    import urllib.request
    urllib2 = urllib.request
import urllib
#import poster
try:
    import web
except:
    class web:
        class Forbidden(BaseException): pass
        class NotFound(BaseException): pass

from pythia.constants import DEFAULT_SERVER

class Client(object):
    """
    Wrap http client, make it easy to use with a REST server
    """

    _status = {
        403: web.Forbidden,
        404: web.NotFound,
    }

    def __init__(self, server=DEFAULT_SERVER, session={}):
        self._session = session
        self._server = server.rstrip('/') + '/'

    def get(self, url, params={}, token=None):
        return self._req("GET", url, params, token or self._session.get('token'))

    def delete(self, url, params={}, token=None):
        return self._req("DELETE", url, params, token or self._session.get('token'))

    def _req(self, method, url, params, token):
        url = self._server + url.lstrip('/') + '?' + urllib.urlencode(params)
        client = httplib2.Http()
        headers = {}
        if token is not None:
            headers['Authorization'] = token
        return self._getContent(*client.request(url, method, headers=headers))

    def post(self, url, params={}, token=None):
        url = self._server + url.lstrip('/')
        try:
            body = urllib.quote(json.dumps(params))
        except:
            body = urllib.parse.quote(json.dumps(params))

        client = httplib2.Http()
        headers = {
            'Content-Type': 'application/x-www-form-urlencoded',
        }
        token = token or self._session.get('token')
        if token is not None:
            headers['Authorization'] = token
        return self._getContent(*client.request(url, 'POST', body=body, headers=headers))

    def _getContent(self, response, content):
        if response.status != 200:
            exception = self._status.get(response.status)
            if exception:
                raise exception()
            else:
                raise Exception(content)
        if content:
            return json.loads(content)
        else:
            return None

    def post_multipart(self, url, params, token=None):
        #opener = poster.streaminghttp.register_openers()
        params['token'] = token or self._session.get('token')
        fields = []
        files = []
        for k, v in params.iteritems():
            if isinstance(v, tuple):
                f, fname = v
                if hasattr(f, 'read'):
                    if hasattr(f, 'fileno'):
                        files.append((k, f, fname))
                    elif isinstance(v, tuple):
                        s = v[0].read()
                        files.append((k, s, v[1]))
                    else:
                        fields.append((k, v.read()))
                else:
                    raise Exception("tuple has to contain a file-like object, and the filename")
            else:
                fields.append((k, v))
        headers, gen = self._genPostData(fields, files)
        request = urllib2.Request(url, gen, headers)
        content = urllib2.urlopen(request)
        if content:
            return json.loads(content.read())
        else:
            return None

    def _genPostData(self, fields, files):
        boundary = "THis_is_A_BOUNDARY_________________<<<<:"
        CRLF = '\r\n'
        start = ''
        for k, v in fields:
            start += CRLF.join([
                '--' + boundary,
                'Content-Disposition: form-data; name="%s"' % k,
                '',
                str(v),
                '',
            ])
        fsize = 0
        to_send = []
        for k, f, fname in files:
            if hasattr(f, 'fileno'):
                size = os.fstat(f.fileno()).st_size
            else:
                size = len(f)
            h = CRLF.join([
                '--' + boundary,
                'Content-Disposition: form-data; name="%s"; filename="%s"' % (k, fname),
                'Content-Type: octet/stream',
                '',
                '',
            ])
            to_send.append((k, f, fname, size, h))
            fsize += size + len(h) + 2

        end = CRLF.join([
            '--' + boundary + '--',
            '',
        ])

        class Yielder(object):
            def __init__(self, start, to_send, end):
                self.start = start
                self.to_send = to_send
                self.end = end
            def __iter__(self):
                self.g = self.gen()
                return self
            def next(self):
                res = self.g.next()
                if res is None:
                    raise StopIteration()
                return res
            def gen(self):
                self._sent = 0
                yield self.start
                for k, f, fname, size, headers in self.to_send:
                    yield headers
                    if isinstance(f, basestring):
                        yield f
                    else:
                        while True:
                            data = f.read(4096)
                            if not data:
                                break
                            yield data
                    yield CRLF
                yield self.end

        form_headers = {
            'Content-Type': "multipart/form-data; boundary=%s" % boundary,
            'Content-Length': str(len(start) + fsize + len(end)),
        }
        return form_headers, Yielder(start, to_send, end)

