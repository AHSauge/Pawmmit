Rationale behind Pawmmit
==================================
This is a fork of Gittyup, and is first and foremost a test-bed for myself (@AHSauge). This includes GitHub itself, hence an organisation. Additionally, this is also a means to provide myself an application with more advanced features as it's part of my professional development workflow, and ensure that it stays secure with relevant patches. If others find the contributions useful, we can discuss how to proceed further.

Generally speaking, the focus is on providing performance, security, and usability improvements, in addition to commercial grade features.

Whilst I will continue to contribute to Gittyup, there are some concrete differences that either already exists or may appear in the future.

1. Branding. This is re-branded to ensure a clear separation from Gittyup and GitAhead, and avoid any confusion on that matter.
2. The codebase is GPLv3 instead of MIT. This provides a stronger assurance that the code, including improvements, stays open-source. For most development I'm fine with MIT licensing (aka. you can ask for approval to upstream things for me at alf.henrik.sauge@gmail.com), but there are some feature I'm uncomfortable releasing under MIT. This includes amongst others
   - Interactive rebase
   - Rewriting the search engine
3. I use this as a technical test-bed for meson. For example, this highlights some friction using Qt in meson, and I'm likely to use this to improve meson
4. GUI changes that are no acceptable upstream
