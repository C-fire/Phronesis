from django.conf.urls import patterns, url
from django.views.generic import DetailView, ListView
from phrSite.models import Recoveryhistory,Metaagent, Server, Dependency, Recoveryrule
from phrSite import views
from django.conf import settings

urlpatterns = patterns('',
    # ex: /polls/
    url(r'^$', views.index, name='index'),
    # ex: /polls/5/
#    url(r'^Metaagent/(?P<ma_id>\d+)/$', views.metaAgent_detail, name='metaAgent_detail'),
#    url(r'^History/$', views.history, name='history'),
    url(r'^Metaagent/$',
        ListView.as_view(
            queryset=Metaagent.objects.order_by('name'),
            context_object_name='metaAgent_list',
            template_name='phrSite/metaAgent.html'),
        name='metaAgent'),
    url(r'^Metaagent/(?P<pk>\d+)/$',
        DetailView.as_view(
            model=Metaagent,
            template_name='phrSite/metaAgent_detail.html'),
        name='metaAgent_detail'),
    url(r'^Metaagenttree/$', views.metaAgentTree, name='metaAgentTree'),
    url(r'^Server/$',
        ListView.as_view(
            queryset=Server.objects.order_by('name'),
            context_object_name='server_list',
            template_name='phrSite/server.html'),
        name='server'),
    url(r'^Server/(?P<pk>\d+)/$',
        DetailView.as_view(
            model=Server,
            template_name='phrSite/server_detail.html'),
        name='server_detail'),
    url(r'^History/$',
        ListView.as_view(
            queryset=Recoveryhistory.objects.order_by('-id'),
            context_object_name='history_list',
            template_name='phrSite/history.html'),
        name='history'),
#    url(r'^History/(?P<pk>\d+)/$',
#        DetailView.as_view(
#            model=Recoveryhistory,
#            template_name='phrSite/history_detail.html'),
#        name='history_detail'),
    url(r'^History/(?P<history_id>\d+)/$', views.history_detail, name='history_detail'),
    url(r'^static/(?P<path>.*)$',
     'django.views.static.serve', {'document_root': settings.STATIC_ROOT}),
    url(r'^Search/$', views.search, name='search'),
#    url(r'^Stream/$', views.stream_response, name='stream'),
    url(r'^Console/$', views.console, name='console'),
#    url(r'^Form/$', views.form, name='form'),
#    url(r'^Form/(?P<history_id>\d+)/$', views.form, name='form'),
    url(r'^Dependency/$',
        ListView.as_view(
            queryset=Dependency.objects.order_by('id'),
            context_object_name='dependency_list',
            template_name='phrSite/dependency.html'),
        name='dependency'),
    url(r'^Dependency/(?P<dependency_id>\d+)/$', views.dependency_validitySwap, name='dependency_validitySwap'),
    url(r'^Trigger/$',
        ListView.as_view(
            queryset=Recoveryrule.objects.order_by('id'),
            context_object_name='trigger_list',
            template_name='phrSite/trigger.html'),
        name='trigger'),
)
