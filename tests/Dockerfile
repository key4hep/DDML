FROM ghcr.io/key4hep/key4hep-images/ubuntu22:latest
COPY entrypoint.sh .
RUN chmod +x ./entrypoint.sh
#ENTRYPOINT ["sh", "-c", "/resources/setup_env_nightly.sh && /entrypoint.sh && whi"]
ENTRYPOINT ["sh", "-c", "/entrypoint.sh"]