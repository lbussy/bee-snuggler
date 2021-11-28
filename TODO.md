# TODO

## Controller Function

- [ ] Allow a non-captive portal
- [ ] Allow local AP access
- [ ] Create STA mode capability (semaphore on boot?)
- [ ] Allow setting time from web page
- [ ] Test OTA
- [ ] Understand if we are offline
    - [ ] Prevent OTA
- [ ] Add "ID Me" button (flashing LED)

## Application Function

## Release +

- [ ] Allow for SSR (1/60 sec) or Relay (15 seconds)

## Notes

Webpage - only online:
```
    <!--
    <script src="//ajax.googleapis.com/ajax/libs/jquery/1/jquery.min.js" type="text/javascript"></script>
    <script type="text/javascript">window.jQuery || document.write('<script type="text/javascript" src="/js/lib/jquery.js">\x3C/script>');</script>
    -->
```