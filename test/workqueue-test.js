'use strict';

const tape = require('tape');

const common = require('./common');

tape('v8 workqueue commands', (t) => {
  t.timeoutAfter(15000);

  const sess = common.Session.create('workqueue-scenario.js');
  sess.timeoutAfter

  sess.waitBreak((err) => {
    t.error(err);
    sess.send('v8 getactivehandles');
  });

  sess.wait(/TCP/, (err, line) => {
    t.error(err);
    let match = line.match(/<Object: TCP/i);
    t.ok(match, 'TCP handler should be an Object');

    sess.send('v8 getactivehandles');
  });

  sess.wait(/Timer/, (err, line) => {
    t.error(err);
    let match = line.match(/<Object: Timer/i);
    t.ok(match, 'Timer handler should be an Object');

    sess.send('v8 getactiverequests');
  });

  sess.wait(/FSReqWrap/, (err, line) => {
    t.error(err);
    let match = line.match(/<Object: FSReqWrap/i);
    t.ok(match, 'FSReqWrap handler should be an Object');

    sess.quit();
    t.end();
  });
});
